#ifndef IO_H
#define IO_H

#include <stdint.h>

unsigned char inb(unsigned short _port);
void outb(unsigned short _port, unsigned char _data);
void outb_slow(unsigned short _port, unsigned char _data);
void outl(uint16_t port, uint32_t data);
void outw(uint16_t port, uint16_t data);
uint32_t inl(unsigned short port);
uint16_t inw(uint16_t _port);

#endif