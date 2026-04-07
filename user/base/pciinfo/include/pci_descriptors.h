#pragma once

#include <stdint.h>


const char* get_vendor_name(uint16_t vendor_ID);
const char* get_device_name(uint16_t vendor_ID, uint16_t device_ID);
const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code);
const char* get_prog_IF_name(uint8_t class_code, uint8_t subclass_code, uint8_t prog_IF);

const char* get_device_class(uint8_t _class);
