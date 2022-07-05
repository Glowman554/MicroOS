#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_FLAG_DATASEG 0x02
#define GDT_FLAG_CODESEG 0x0a
#define GDT_FLAG_TSS	 0x09

#define GDT_FLAG_SEGMENT 0x10
#define GDT_FLAG_RING0   0x00
#define GDT_FLAG_RING3   0x60
#define GDT_FLAG_PRESENT 0x80

#define GDT_FLAG_4K	  0x800
#define GDT_FLAG_32_BIT  0x400

#define GDT_ENTRIES 6

void set_tss(int index, uint32_t val);

void init_gdt();

#endif