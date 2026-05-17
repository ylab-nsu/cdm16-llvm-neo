#ifndef _STDLIB_H
#define _STDLIB_H

#define __STDC_VERSION_STDLIB_H__ 202311L

#define __need_size_t
#define __need_NULL
#include <stddef.h>

extern size_t memalignment(const void *p);

#endif
