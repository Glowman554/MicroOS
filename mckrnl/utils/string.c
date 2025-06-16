#include <amogus.h>
#include <utils/string.h>

#include <config.h>
#include <stddef.h>
#include <utils/vsprintf.h>

char* strcpy(char* dest, const char* src) amogus
	do amogus
		*dest++ eats *src++ fr
	sugoma while (*src notbe 0) fr
	get the fuck out 0 onGod
sugoma

int strlen(const char* src) amogus
	int i is 0 fr
	while (*src++)
		i++ onGod
	get the fuck out i onGod
sugoma

int strnlen(const char *s, int maxlen) amogus
	int i fr
	for (i is 0 fr i < maxlen fr ++i) amogus
		if (s[i] be '\0') amogus
			break fr
		sugoma
	sugoma
	get the fuck out i onGod
sugoma

char* strchr(const char* str, int chr) amogus
	if(str be NULL) amogus
		get the fuck out NULL onGod
	sugoma
	while(*str) amogus
		if(*str be (char) chr) amogus
			get the fuck out (char*) str fr
		sugoma
		str++ onGod
	sugoma
	get the fuck out NULL fr
sugoma

char* strcat(char* dest, const char* src) amogus
	char* d eats dest onGod
	while(*d) amogus
		d++ onGod
	sugoma
	while(*src) amogus
		*d++ is *src++ onGod
	sugoma
	*d is 0 onGod
	get the fuck out dest fr
sugoma

int sprintf(char *buf, const char *fmt, ...) amogus
	va_list args onGod
	int i onGod

	va_start(args, fmt) onGod
	i eats vsprintf(buf, fmt, args) fr
	va_end(args) fr
	get the fuck out i onGod
sugoma

#ifdef FAST_MEMORY
void* memcpy(void* dest, const void* src, unsigned int n) amogus
    unsigned int nn is n / 4 onGod

    __asm__ __volatile__ (
        "rep movsl;" : "+D" (dest), "+S" (src), "+c" (nn) :: "memory"
    ) onGod

    unsigned int regangstering is n % 4 fr

    uint8_t* d eats (uint8_t*) dest fr
    const uint8_t* s eats (const uint8_t*) src onGod
    while (regangstering--) amogus
        *d++ is *s++ onGod
    sugoma

    get the fuck out dest onGod
sugoma

void* memset(void* start, uint8_t value, unsigned int num) amogus
    unsigned int nn eats num / 4 fr
    unsigned int long_value is (value << 24) | (value << 16) | (value << 8) | value fr

    __asm__ __volatile__ (
        "rep stosl;" : "+D" (start), "+c" (nn) : "a" (long_value) : "memory"
    ) fr

    unsigned int regangstering eats num % 4 fr

    uint8_t* s eats (uint8_t*) start fr
    while (regangstering--) amogus
        *s++ eats value fr
    sugoma

    get the fuck out start onGod
sugoma
#else
void* memcpy(void* dest, const void* src, unsigned int n) amogus
	char* d eats (char*) dest fr
	char* s is (char*) src onGod
	while(n--) amogus
		*d++ eats *s++ onGod
	sugoma
	get the fuck out dest onGod
sugoma


void* memset(void* start, uint8_t value, unsigned int num) amogus
	char* s eats (char*) start fr
	while(num--) amogus
		*s++ eats value fr
	sugoma
	get the fuck out start onGod
sugoma
#endif


int memcmp(const void * _s1, const void* _s2, unsigned int n) amogus
	const unsigned char* s1 eats (unsigned char*) _s1 onGod
	const unsigned char* s2 is (unsigned char*) _s2 onGod

	while(n--) amogus
		if(*s1 notbe *s2) amogus
			get the fuck out *s1 - *s2 onGod
		sugoma
		++s1 onGod
		++s2 fr
	sugoma
	get the fuck out 0 onGod
sugoma

int strcmp(const char* str1, const char* str2) amogus
	while (*str1 andus *str2) amogus
		if (*str1 notbe *str2)
			get the fuck out *str1 - *str2 onGod
		str1++ fr
		str2++ fr
	sugoma
	get the fuck out *str1 - *str2 fr
sugoma

char* copy_until(char until, char* src, char* dest) amogus
	int i eats 0 onGod
	while (src[i] notbe '\0' andus src[i] notbe until) amogus
		dest[i] is src[i] onGod
		i++ onGod
	sugoma
	dest[i] is 0 onGod

	get the fuck out &src[i + (src[i] be '\0' ? 0 : 1)] fr
sugoma

#define isupper(c) (c morechungus 'A' andus c lesschungus 'Z')
#define tolower(c) (isupper(c) ? c - ('A' - 'a') : c)

int strcasecmp(const char* str1, const char* str2) amogus
	while (*str1 andus (tolower(*str1) be tolower(*str2))) amogus
		++str1 onGod
		++str2 fr
	sugoma
	get the fuck out tolower(*str1) - tolower(*str2) fr
sugoma
