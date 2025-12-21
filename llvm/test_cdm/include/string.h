#ifndef FB7EC850_A287_4745_A5C1_2DA599F2D018
#define FB7EC850_A287_4745_A5C1_2DA599F2D018

#include "stddef.h"


/**
 * Searches for the first occurrence of the character c (an unsigned char) in
 * the first n bytes of the string pointed to, by the argument str.
 */
void *memchr(const void *str, int c, size_t n);

/**
 * Compares the first n bytes of str1 and str2.
 */
int memcmp(const void *str1, const void *str2, size_t n);

/**
 * Copies n characters from src to dest.
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * Copies the character c (an unsigned char) to the first n characters of the
 * string pointed to, by the argument str.
 */
void *memset(void *str, int c, size_t n);

/**
 * Appends the string pointed to, by src to the end of the string pointed to, by
 * dest up to n characters long.
 */
char *strncat(char *dest, const char *src, size_t n);

/**
 * Searches for the first occurrence of the character c (an unsigned char) in
 * the string pointed to, by the argument str.
 */
char *strchr(const char *str, int c);

/**
 * Compares at most the first n bytes of str1 and str2.
 */
int strncmp(const char *str1, const char *str2, size_t n);

/**
 * Copies up to n characters from the string pointed to, by src to dest.
 */
char *strncpy(char *dest, const char *src, size_t n);

/**
 * Calculates the length of the initial segment of str1 which consists entirely
 * of characters not in str2.
 */
size_t strcspn(const char *str1, const char *str2);

/**
 * Computes the length of the string str up to but not including the terminating
 * null character.
 */
size_t strlen(const char *str);

/**
 * Finds the first character in the string str1 that matches any character
 * specified in str2.
 */
char *strpbrk(const char *str1, const char *str2);

/**
 * Searches for the last occurrence of the character c (an unsigned char) in the
 * string pointed to by the argument str.
 */
char *strrchr(const char *str, int c);

/**
 * Finds the first occurrence of the entire string needle (not including the
 * terminating null character) which appears in the string haystack.
 */
char *strstr(const char *haystack, const char *needle);

/**
 * Breaks string str into a series of tokens separated by delim.
 */
char *strtok(char *str, const char *delim);


#endif/* FB7EC850_A287_4745_A5C1_2DA599F2D018 */
