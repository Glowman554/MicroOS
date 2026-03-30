#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <stddef.h>
#include <stdint.h>

/* basic POSIX types */
typedef int32_t  pid_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t mode_t;

/* filesystem related */
typedef int32_t  off_t;
typedef uint32_t dev_t;
typedef uint32_t ino_t;
typedef uint32_t nlink_t;

/* block counts */
typedef int32_t  blksize_t;
typedef int32_t  blkcnt_t;

/* time */
typedef int32_t  time_t;

/* signed size */
typedef int32_t ssize_t;

#endif