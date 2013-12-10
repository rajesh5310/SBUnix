#ifndef ASSERT_H
#define ASSERT_H

#include <sys/print.h>

#define assert(x)		\
    do { if (!(x)) print("assertion failed: %s", #x); } while (0)

// static_assert(x) will generate a compile-time error if 'x' is false.
#define static_assert(x)	switch (x) case 0: case (x):

#endif /* ASSERT_H */
