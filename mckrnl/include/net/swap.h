#pragma once

#define BSWAP16(n) ((n & 0x00FF) << 8) | ((n & 0xFF00) >> 8)
#define BSWAP32(n)  ((n & 0xFF000000) >> 24) | ((n & 0x00FF0000) >> 8) | ((n & 0x0000FF00) << 8) | ((n & 0x000000FF) << 24)
