#ifndef DEFS_H
#define DEFS_H

#ifndef NULL
#define NULL ((void*) 0)
#endif

// Represents true-or-false values
typedef _Bool bool;
enum { false, true };

// Explicitly-sized versions of integer types

typedef __signed char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;


typedef char                sint8_t;
typedef short               sint16_t;
typedef int                 sint32_t;
typedef long long           sint64_t;


// Pointers and addresses are 64 bits long.
// We use pointer types to represent virtual addresses,
// uintptr_t to represent the numerical values of virtual addresses,
// and physaddr_t to represent physical addresses.

typedef int64_t intptr_t;
typedef uint64_t uintptr_t;
typedef uint64_t physaddr_t;

// Page numbers are 64 bits long.

typedef uint64_t ppn_t;

// size_t is used for memory object sizes.
typedef uint64_t size_t;
// ssize_t is a signed version of ssize_t, used in case there might be an
// error return.
typedef int64_t ssize_t;

// off_t is used for file offsets and lengths.
typedef int32_t off_t;

// Efficient min and max operations
#define MIN(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})
#define MAX(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

// Rounding operations (efficient when n is a power of 2)
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n)						\
({								\
	uint64_t __a = (uint64_t) (a);				\
	(typeof(a)) (__a - __a % (n));				\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)						\
({								\
	uint64_t __n = (uint64_t) (n);				\
	(typeof(a)) (ROUNDDOWN((uint64_t) (a) + __n - 1, __n));	\
})

// Return the offset of 'member' relative to the beginning of a struct type
#define offsetof(type, member)  ((size_t) (&((type*)0)->member))
typedef struct {
    int inode_num;//This field will be index in the tarfs table
    char filename[100];
    int perm;
    int size;
    char type;
    int sector_loc[10];
    int offset;
    uint64_t address;
    int is_fs;
} file;

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

#endif /* DEFS_H */
