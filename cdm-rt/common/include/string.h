#ifndef _STRING_H
#define _STRING_H

#define __STDC_VERSION_STRING_H__ 202311L

#define	__need_size_t
#define	__need_NULL
#include <stddef.h>

extern void *memcpy(void *restrict s1, const void *restrict s2, size_t n);

extern void *memccpy(void *restrict s1, const void *restrict s2, int c, size_t n);

extern void *memmove(void *s1, const void *s2, size_t n);

extern char *strcpy(char *restrict s1, const char *restrict s2);

extern char *strncpy(char *restrict s1, const char *restrict s2, size_t n);

extern char *strcat(char *restrict s1, const char *restrict s2);

extern char *strncat(char *restrict s1, const char *restrict s2, size_t n);

extern int memcmp(const void *s1, const void *s2, size_t n);

extern int strcmp(const char *s1, const char *s2);

extern int strncmp(const char *s1, const char *s2, size_t n);

extern void *memchr(void *s, int c, size_t n);

extern char *strchr(char *s, int c);

extern size_t strcspn(const char *s1, const char *s2);

extern char *strpbrk(char *s1, const char *s2);

extern char *strrchr(char *s, int c);

extern size_t strspn(const char *s1, const char *s2);

extern char *strstr(char *s1, const char *s2);

extern void *memset(void *s, int c, size_t n);

extern void *memset_explicit(void *s, int c, size_t n);

extern size_t strlen(const char *s);

#endif
