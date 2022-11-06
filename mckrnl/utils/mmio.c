#include <utils/mmio.h>

uint8_t mmio_read8(uint32_t address) {
	return *((volatile uint8_t*) address);
}

uint16_t mmio_read16(uint32_t address) {
	return *((volatile uint16_t*) address);
}

uint32_t mmio_read32(uint32_t address) {
	return *((volatile uint32_t*) address);
}

void mmio_write8(uint32_t address, uint8_t value) {
	(*((volatile uint8_t*) address)) = value;
}

void mmio_write16(uint32_t address, uint16_t value) {
	(*((volatile uint16_t*) address)) = value;    
}

void mmio_write32(uint32_t address, uint32_t value) {
	(*((volatile uint32_t*) address)) = value;
}
