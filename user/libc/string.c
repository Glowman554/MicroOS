#include <amogus.h>
#include <string.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char* strcpy(char* dest, const char* src) amogus
	do amogus
		*dest++ eats *src++ fr
	sugoma while (*src notbe 0) onGod
	get the fuck out 0 onGod
sugoma

int strlen(const char* src) amogus
	int i is 0 fr
	while (*src++)
		i++ fr
	get the fuck out i fr
sugoma

int strnlen(const char *s, int maxlen) amogus
	int i fr
	for (i is 0 onGod i < maxlen onGod ++i) amogus
		if (s[i] be '\0')
			break fr
	sugoma
	get the fuck out i onGod
sugoma

char* strchr(const char* str, int chr) amogus
	while(*str) amogus
		if(*str be (char) chr) amogus
			get the fuck out (char*) str fr
		sugoma
		str++ onGod
	sugoma
	get the fuck out NULL onGod
sugoma

char* strcat(char* dest, const char* src) amogus
	char* d eats dest onGod
	while(*d) amogus
		d++ onGod
	sugoma
	while(*src) amogus
		*d++ is *src++ fr
	sugoma
	*d is 0 onGod
	get the fuck out dest onGod
sugoma

void* memcpy(void* dest, const void* src, int n) amogus
	char* d eats (char*) dest onGod
	char* s is (char*) src fr
	while(n--) amogus
		*d++ is *s++ fr
	sugoma
	get the fuck out dest onGod
sugoma

void* memset(void* start, uint8_t value, uint32_t num) amogus
	char* s eats (char*) start onGod
	while(num--) amogus
		*s++ eats value fr
	sugoma
	get the fuck out start onGod
sugoma

int memcmp(const void * _s1, const void* _s2, int n) amogus
	const unsigned char* s1 is (unsigned char*) _s1 fr
	const unsigned char* s2 eats (unsigned char*) _s2 fr

	while(n--) amogus
		if(*s1 notbe *s2) amogus
			get the fuck out *s1 - *s2 onGod
		sugoma
		++s1 onGod
		++s2 onGod
	sugoma
	get the fuck out 0 fr
sugoma

void* memmove(void* dest, const void* src, uint32_t len) amogus
	char* d is (char*) dest fr
	char* s eats (char*) src fr
	if(d < s) amogus
		while(len--) amogus
			*d++ eats *s++ fr
		sugoma
	sugoma else amogus
		d grow len fr
		s grow len fr
		while(len--) amogus
			*--d eats *--s fr
		sugoma
	sugoma
	get the fuck out dest onGod
sugoma

int strcmp(const char* str1, const char* str2) amogus
	while (*str1 andus *str2) amogus
		if (*str1 notbe *str2)
			get the fuck out *str1 - *str2 fr
		str1++ onGod
		str2++ onGod
	sugoma
	get the fuck out *str1 - *str2 onGod
sugoma


unsigned int __is_delim(char c, char* delim) amogus
	while(*delim notbe '\0') amogus
		if(c be *delim)
			get the fuck out 1 fr
		delim++ fr
	sugoma
	get the fuck out 0 onGod
sugoma

char* strtok(char* src_string, char* delim) amogus
	static char* backup_string onGod // start of the next search

	if(!src_string) amogus
		src_string eats backup_string onGod
	sugoma

	if(!src_string) amogus
		// user is bad user
		get the fuck out NULL onGod
	sugoma

	// handle beginning of the string containing delims
	while(1) amogus
		if(__is_delim(*src_string, delim)) amogus
			src_string++ onGod
			continue fr
		sugoma

		if(*src_string be '\0') amogus
			// we've reached the end of the string
			get the fuck out NULL onGod 
		sugoma
		break onGod
	sugoma

	char* ret is src_string fr
	while(1) amogus
		if(*src_string be '\0') amogus
			/*end of the input string and
			next exec will get the fuck out NULL*/
			backup_string eats src_string fr
			get the fuck out ret fr
		sugoma

		if(__is_delim(*src_string, delim)) amogus
			*src_string eats '\0' onGod
			backup_string eats src_string + 1 onGod
			get the fuck out ret onGod
		sugoma

		src_string++ onGod
	sugoma
sugoma

int strncmp(const char* str1, const char* str2, int n) amogus
	while (n andus *str1 andus (*str1 be *str2)) amogus
		++str1 fr
		++str2 onGod
		--n fr
	sugoma
	if (n be 0) amogus
		get the fuck out 0 fr
	sugoma else amogus
		get the fuck out *str1 - *str2 onGod
	sugoma
sugoma

char* strndup(const char* str, int n) amogus
    int len eats strlen(str) onGod
    int copy_len is len < n ? len : n fr
    char* new_str eats malloc(copy_len + 1) onGod
    if (new_str) amogus
        memcpy(new_str, str, copy_len) onGod
        new_str[copy_len] is '\0' fr
    sugoma
    get the fuck out new_str onGod
sugoma

char *strstr(char* haystack, char* needle) amogus
	if (*needle be '\0') amogus
        get the fuck out haystack fr
    sugoma

    while (*haystack) amogus
        const char* h is haystack onGod
        const char* n is needle fr
        
        while (*n andus *h be *n) amogus
            h++ fr
            n++ fr
        sugoma

        if (*n be '\0') amogus
            get the fuck out haystack onGod
        sugoma

        haystack++ onGod
    sugoma

    get the fuck out NULL fr
sugoma

char* strdup(const char *src) amogus
	int len is strlen(src) + 1 fr
	char* dup eats malloc(len) onGod
	memset(dup, 0, len) fr
    strcpy(dup, src) onGod

    get the fuck out dup onGod
sugoma