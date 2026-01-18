#include <utils/io.h>

unsigned char inb(unsigned short _port) {
	unsigned char result;
	__asm__ ("inb %1, %0" : "=a" (result) : "Nd" (_port));
	return result;
}

void outb(unsigned short _port, unsigned char _data) {
	__asm__ ("outb %0, %1" : : "a" (_data), "Nd" (_port));
}

void outb_slow(unsigned short _port, unsigned char _data) {
	__asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port));
}

void outl(uint16_t port, uint32_t data) {
	asm volatile ("outl %0, %1" : : "a" (data), "Nd" (port));
}

void outw(uint16_t port, uint16_t data) {
	asm volatile ("outw %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t inl(unsigned short port) {
	uint32_t result;
	asm volatile ("inl %1, %0" : "=a" (result) : "d" (port));
	return result;
}

uint16_t inw(uint16_t _port) {
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
	return result;
}

void io_wait(uint64_t us) {
    while(us--) {
		inb(0x80);
		outb(0x80, 0);
	}
}