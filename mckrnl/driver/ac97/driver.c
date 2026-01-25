#include <ac97.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <interrupts/interrupts.h>
#include <driver/timer_driver.h>
#include <scheduler/async.h>

void ac97_housekeeping(async_t* asnyc);

int min(int a, int b) {
    return (a <= b) ? a : b;
}

void ac97_irq(cpu_registers_t* regs, void* ac97V) {}

char* ac97_get_device_name(driver_t* driver) {
    return "ac97";
}

bool ac97_is_device_present(driver_t* driver) {
    return true;
}

void ac97_init(driver_t* driver) {
    ac97_driver_t* ac97 = (ac97_driver_t*) driver;

    disable_io(ac97->bus, ac97->device, ac97->function);
    disable_mmio(ac97->bus, ac97->device, ac97->function);

    enable_interrupt(ac97->bus, ac97->device, ac97->function);
    enable_bus_master(ac97->bus, ac97->device, ac97->function);

    if (ac97->header.BAR1 == 0) {
        abortf(false, "AC97 Driver only uses BAR0 and is not supported");
    }

    enable_io(ac97->bus, ac97->device, ac97->function);
    enable_mmio(ac97->bus, ac97->device, ac97->function);

    pci_bar_t bar = pci_get_bar(&ac97->header.BAR0, 0, ac97->bus, ac97->device, ac97->function);
    debugf("AC97 MEM: %x. IO: %x, SIZE: %x, TYPE: %d", (uint32_t)bar.mem_address, bar.io_address, bar.size, bar.type);

    uint8_t irq = ac97->header.interrupt_line;
    ac97->irq = irq + 0x20;
    register_interrupt_handler(ac97->irq, (void*)&ac97_irq, (void*)ac97);

    pci_device_header_t hdr = ac97->header;
    ac97->m_mixer_type = pci_get_bar(&hdr.BAR0, 0, ac97->bus, ac97->device, ac97->function);
    
    if (ac97->m_mixer_type.type == MMIO64) {
        ac97->m_bus_type = pci_get_bar(&hdr.BAR0, 2, ac97->bus, ac97->device, ac97->function);
    } else {
        ac97->m_bus_type = pci_get_bar(&hdr.BAR0, 1, ac97->bus, ac97->device, ac97->function);
    }
    
    ac97->m_output_channel = NABM_PCM_OUT;

    ac97->m_output_buffer_region = (uint8_t*)vmm_alloc(AC97_OUTPUT_BUFFER_PAGES);
    memset(ac97->m_output_buffer_region, 0, 0x1000 * AC97_OUTPUT_BUFFER_PAGES);

    ac97->m_output_buffer_descriptor_region = vmm_alloc(1);
    memset(ac97->m_output_buffer_descriptor_region, 0, sizeof(ac97_buffer_descriptor_t) * AC97_NUM_BUFFER_DESCRIPTORS);

    ac97->m_output_buffer_descriptors = (ac97_buffer_descriptor_t*) ac97->m_output_buffer_descriptor_region;

    int control = pci_bar_read_dword(ac97->m_bus_type, GLOBAL_CONTROL);
    control |= COLD_RESET | INTERRUPT_ENABLE;
    control &= ~(0b11 << 22);
    pci_bar_write_dword(ac97->m_bus_type, GLOBAL_CONTROL, control);
    ac97_write_mixer(ac97, RESET, 1);

    ac97_write_mixer(ac97, MASTER_VOLUME, 0);
    ac97_write_mixer(ac97, PCM_VOLUME, 0x0808);
    ac97_reset_output(ac97);

    const int wanted_sample_rate = 48000;
    int tries = 10;
    while (tries-- > 0) {
        ac97_set_sample_rate(ac97, wanted_sample_rate);
        if (ac97->m_sample_rate == wanted_sample_rate) {
            break;
        }
        io_wait(1000);
    }

    if (ac97->m_sample_rate != wanted_sample_rate) {
        abortf(false, "Failed to set sample rate (%d)", ac97->m_sample_rate);
    }

    ac97_reset_output(ac97);

    ac97->pending_audio_data = NULL;
    ac97->pending_audio_size = 0;
    ac97->pending_audio_offset = 0;

    async_t async = {
        .state = STATE_WAIT,
        .data = (void*)ac97,
    };
    add_async_task(&ac97_housekeeping, async, false);

    global_sound_driver = (sound_driver_t*)ac97;

    debugf("AC97 ready");
}

uint64_t ac97_write_pcm(sound_driver_t* driver, uint8_t* buffer, uint64_t size) {
    ac97_driver_t* ac97 = (ac97_driver_t*)driver;
    debugf("ac97_write_pcm(0x%x, %d)", buffer, size);
    return ac97_write_buffer(ac97, 0, buffer, size);
}

uint32_t ac97_get_sample_rate(sound_driver_t* driver) {
    ac97_driver_t* ac97 = (ac97_driver_t*)driver;
    return ac97->m_sample_rate;
}

uint64_t ac97_write_buffer_chunk(ac97_driver_t* driver, uint8_t* buffer, uint64_t count, int max_buffers) {
    uint64_t n_written = 0;
    int buffers_written = 0;

    while (count > 0 && buffers_written < max_buffers) {
        if (!driver->m_output_dma_enabled) {
            ac97_reset_output(driver);
        }

        uint32_t* output_buffer = (uint32_t*)(driver->m_output_buffer_region + 0x1000 * driver->m_current_output_buffer_page);
        size_t num_bytes = min(count, 0x1000);
        memcpy(output_buffer, buffer + n_written, num_bytes);
        count -= num_bytes;
        n_written += num_bytes;

        ac97_buffer_descriptor_t* descriptor = &driver->m_output_buffer_descriptors[driver->m_current_buffer_descriptor];
        descriptor->data_addr = (uint32_t)(((uint64_t)driver->m_output_buffer_region) + 0x1000 * driver->m_current_output_buffer_page);
        descriptor->num_samples = num_bytes / sizeof(uint16_t);
        descriptor->flags.is_last_entry = false;
        descriptor->flags.interrupt_on_completion = true;

        pci_bar_write_dword(driver->m_bus_type, driver->m_output_channel + BUFFER_LIST_ADDR, (uint32_t)(uint64_t)driver->m_output_buffer_descriptor_region);
        pci_bar_write_byte(driver->m_bus_type, driver->m_output_channel + LAST_VALID_INDEX, driver->m_current_buffer_descriptor);

        if (!driver->m_output_dma_enabled) {
            uint8_t ctrl = pci_bar_read_byte(driver->m_bus_type, driver->m_output_channel + CONTROL);
            ctrl |= PAUSE_BUS_MASTER | ERROR_INTERRUPT | COMPLETION_INTERRUPT;
            pci_bar_write_byte(driver->m_bus_type, driver->m_output_channel + CONTROL, ctrl);
            driver->m_output_dma_enabled = true;
        }

        driver->m_current_output_buffer_page++;
        driver->m_current_output_buffer_page %= AC97_OUTPUT_BUFFER_PAGES;
        driver->m_current_buffer_descriptor++;
        driver->m_current_buffer_descriptor %= AC97_NUM_BUFFER_DESCRIPTORS;
        buffers_written++;
    }

    return n_written;
}

uint64_t ac97_write_buffer(ac97_driver_t* driver, uint64_t offset, uint8_t* buffer, uint64_t count) {
    buffer += offset;
    count -= offset;

    if (driver->pending_audio_data != NULL) {
        uint32_t pages = (driver->pending_audio_size + 0xFFF) / 0x1000;
        if (pages == 0) {
            pages = 1;
        }
        vmm_free(driver->pending_audio_data, pages);
        driver->pending_audio_data = NULL;
    }

    uint32_t pages = (count + 0xFFF) / 0x1000;
    if (pages == 0) {
        pages = 1;
    }
    uint8_t* audio_copy = (uint8_t*)vmm_alloc(pages);
    if (!audio_copy) {
        return 0;
    }
    memcpy(audio_copy, buffer, count);

    driver->pending_audio_data = audio_copy;
    driver->pending_audio_size = count;
    driver->pending_audio_offset = 0;

    uint64_t initial_write = ac97_write_buffer_chunk(driver, audio_copy, count, AC97_NUM_BUFFER_DESCRIPTORS / 2);
    driver->pending_audio_offset = initial_write;

    return count;
}

void ac97_feed_pending(ac97_driver_t* driver) {
    if (!driver->pending_audio_data || driver->pending_audio_offset >= driver->pending_audio_size) {
        return;
    }

    uint8_t current_index = pci_bar_read_byte(driver->m_bus_type, driver->m_output_channel + CURRENT_INDEX);
    uint8_t last_valid_index = pci_bar_read_byte(driver->m_bus_type, driver->m_output_channel + LAST_VALID_INDEX);

    int buffers_playing = (last_valid_index - current_index + AC97_NUM_BUFFER_DESCRIPTORS + 1) % AC97_NUM_BUFFER_DESCRIPTORS;
    int buffers_available = AC97_NUM_BUFFER_DESCRIPTORS - buffers_playing - 2;

    if (buffers_available <= 0) {
        return;
    }

    uint8_t* data = driver->pending_audio_data + driver->pending_audio_offset;
    uint64_t remaining = driver->pending_audio_size - driver->pending_audio_offset;
    uint64_t written = ac97_write_buffer_chunk(driver, data, remaining, buffers_available);
    driver->pending_audio_offset += written;

    if (driver->pending_audio_offset >= driver->pending_audio_size) {
        uint32_t pages = (driver->pending_audio_size + 0xFFF) / 0x1000;
        if (pages == 0) {
            pages = 1;
        }
        vmm_free(driver->pending_audio_data, pages);
        driver->pending_audio_data = NULL;
        driver->pending_audio_size = 0;
        driver->pending_audio_offset = 0;
    }
}

void ac97_write_mixer(ac97_driver_t* driver, uint16_t reg, uint16_t val) {
    pci_bar_write_word(driver->m_mixer_type, reg, val);
    io_wait(10);
}

void ac97_reset_output(ac97_driver_t* driver) {
    int timeOut = 200;
    do {
        pci_bar_write_byte(driver->m_bus_type, driver->m_output_channel + CONTROL, RESET_REGISTERS);
        io_wait(10);
    } while ((timeOut-- > 0) && (pci_bar_read_byte(driver->m_bus_type, driver->m_output_channel + CONTROL) & RESET_REGISTERS));

    driver->m_output_dma_enabled = false;
    driver->m_current_buffer_descriptor = 0;
}

void ac97_set_sample_rate(ac97_driver_t* driver, uint32_t sample_rate) {
    pci_bar_write_word(driver->m_mixer_type, SAMPLE_RATE, sample_rate);
    io_wait(100);
    driver->m_sample_rate = pci_bar_read_word(driver->m_mixer_type, SAMPLE_RATE);
}

bool ac97_handle_irq(ac97_driver_t* driver) {
    uint16_t status_byte = pci_bar_read_word(driver->m_bus_type, driver->m_output_channel + STATUS);
    ac97_buffer_status_t status = {.value = status_byte};

    if (status.fifo_error) {
        abortf(false, "AC97 fifo error!");
    }

    status.value = 0;
    status.completion_interrupt_status = true;
    status.last_valid_interrupt = true;
    status.fifo_error = true;
    pci_bar_write_word(driver->m_bus_type, driver->m_output_channel + STATUS, status.value);

    if (!status.completion_interrupt_status) {
        return false;
    }

    return false;
}


void ac97_housekeeping(async_t* async) {
    ac97_driver_t* driver = (ac97_driver_t*)async->data;

    ac97_feed_pending(driver);
    ac97_handle_irq(driver);
}

sound_driver_t* get_ac97_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
    ac97_driver_t* driver = (ac97_driver_t*)vmm_alloc(PAGES_OF(ac97_driver_t));
    driver->driver.driver.init = ac97_init;
    driver->driver.driver.get_device_name = ac97_get_device_name;
    driver->driver.driver.is_device_present = ac97_is_device_present;
    driver->driver.write_pcm = ac97_write_pcm;
    driver->driver.get_sample_rate = ac97_get_sample_rate;

    driver->header = header;
    driver->bus = bus;
    driver->device = device;
    driver->function = function;

    return (sound_driver_t*) driver;
}

void ac97_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
    register_driver((driver_t*) get_ac97_driver(header, bus, device, function));
}

