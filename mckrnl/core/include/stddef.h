#ifndef STDDEF_H
#define STDDEF_H

typedef __SIZE_TYPE__ size_t;
#define NULL ((void*) 0)

#define offsetof(type, member) __builtin_offsetof(type, member)

#endif