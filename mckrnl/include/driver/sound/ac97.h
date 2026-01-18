#pragma once
#include <driver/pci/pci.h>
#include <driver/pci/pci_bar.h>
#include <driver/sound_driver.h>
#include <stdbool.h>
#include <stdint.h>
#include <utils/io.h>

sound_driver_t* get_ac97_driver(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);
void ac97_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function);

#define AC97_OUTPUT_BUFFER_PAGES 32
#define AC97_NUM_BUFFER_DESCRIPTORS 32

#define NABM_PCM_IN       0x00u
#define NABM_PCM_OUT      0x10u
#define NABM_MIC          0x20u
#define GLOBAL_CONTROL    0x2Cu
#define GLOBAL_STATUS     0x30u

#define RESET             0x00u 
#define MASTER_VOLUME     0x02u 
#define MIC_VOLUME        0x0Eu 
#define PCM_VOLUME        0x18u 
#define INPUT_DEVICE      0x1Au 
#define INPUT_GAIN        0x1Cu 
#define MIC_GAIN          0x1Eu 
#define SAMPLE_RATE       0x2Cu 

#define INTERRUPT_ENABLE  0b00000001
#define COLD_RESET        0b00000010

#define BUFFER_LIST_ADDR  0x00u  
#define CURRENT_INDEX     0x04u  
#define LAST_VALID_INDEX  0x05u  
#define STATUS            0x06u 
#define BUFFER_POSITION   0x08u 
#define PREFETCHED_INDEX  0x0Au  
#define CONTROL           0x0Bu  

#define PAUSE_BUS_MASTER     0b00000001
#define RESET_REGISTERS      0b00000010
#define ERROR_INTERRUPT      0b00001000
#define COMPLETION_INTERRUPT 0b00010000

typedef struct ac97_buffer_descriptor {
	uint32_t data_addr;
	uint16_t num_samples;
	struct {
		uint16_t : 14;
		bool is_last_entry : 1;
		bool interrupt_on_completion : 1;
	} __attribute__((packed)) flags;
} __attribute__((packed)) ac97_buffer_descriptor_t;

typedef union ac97_buffer_status {
	struct {
		bool is_halted : 1;
		bool is_last_valid : 1;
		bool last_valid_interrupt : 1;
		bool completion_interrupt_status : 1;
		bool fifo_error : 1;
		uint16_t : 11;
	};
	uint16_t value;
} ac97_buffer_status_t;

typedef struct ac97_driver {
	sound_driver_t driver;

	pci_bar_t m_mixer_type, m_bus_type;
	ac97_buffer_descriptor_t *m_output_buffer_descriptors;

	pci_device_header_t header;
	uint16_t bus;
	uint16_t device;
	uint16_t function;

	uint16_t m_output_channel;
	uint8_t *m_output_buffer_region;
	uint8_t *m_output_buffer_descriptor_region;

	uint32_t m_current_output_buffer_page;
	uint32_t m_current_buffer_descriptor;
	bool m_output_dma_enabled;
	uint32_t m_sample_rate;
	uint8_t irq;

	uint8_t* pending_audio_data;
	uint32_t pending_audio_size;
	uint32_t pending_audio_offset;
} ac97_driver_t;

void ac97_reset_output(ac97_driver_t* driver);
void ac97_set_sample_rate(ac97_driver_t* driver, uint32_t sample_rate);
uint64_t ac97_write_buffer(ac97_driver_t* driver, uint64_t offset, uint8_t* buffer, uint64_t count);
void ac97_feed_pending(ac97_driver_t* driver);
void ac97_write_mixer(ac97_driver_t* driver, uint16_t reg, uint16_t val);