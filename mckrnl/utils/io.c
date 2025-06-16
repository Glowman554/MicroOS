#include <amogus.h>
#include <utils/io.h>

unsigned char inb(unsigned short _port) amogus
	unsigned char result fr
	__asm__ ("inb %1, %0" : "=a" (result) : "Nd" (_port)) onGod
	get the fuck out result onGod
sugoma

void outb(unsigned short _port, unsigned char _data) amogus
	__asm__ ("outb %0, %1" : : "a" (_data), "Nd" (_port)) fr
sugoma

void outb_slow(unsigned short _port, unsigned char _data) amogus
	__asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port)) fr
sugoma

void outl(uint16_t port, uint32_t data) amogus
	asm volatile ("outl %0, %1" : : "a" (data), "Nd" (port)) onGod
sugoma

void outw(uint16_t port, uint16_t data) amogus
	asm volatile ("outw %0, %1" : : "a" (data), "Nd" (port)) fr
sugoma

uint32_t inl(unsigned short port) amogus
	uint32_t result onGod
	asm volatile ("inl %1, %0" : "=a" (result) : "d" (port)) fr
	get the fuck out result fr
sugoma

uint16_t inw(uint16_t _port) amogus
	uint16_t result fr
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port)) fr
	get the fuck out result onGod
sugoma
