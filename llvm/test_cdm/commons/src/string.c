#include "string.h"

void *memcpy(void *dest, const void *src, size_t n) {
  for (int i = 0; i < (int)n; i++) {
    ((char *)dest)[i] = ((char *)src)[i];
  }
  return dest;
}

size_t strlen(const char* str) {
  size_t length = 0;
  while (*(str + length)) length++;
  return length;
}

void *memchr(const void *stroc, int c, size_t n) {
  const void *res = NULL;
  for (size_t i = 0; i < n && res == NULL; i++) {
    if (((char *)stroc)[i] == c) {
      res = (char *)stroc + i;
    }
  }
  return (void *)res;
}

int memcmp(const void *arr1, const void *arr2, size_t n) {
  int res = 0;
  const unsigned char *ptr1, *ptr2;
  ptr1 = (const unsigned char *)arr1;
  ptr2 = (const unsigned char *)arr2;

  while (n) {
    res = *ptr1 - *ptr2;
    if (res) break;
    ptr1++;
    ptr2++;
    n--;
  }

  return res;
}

void *memset(void *str, int c, size_t n) {
  for (int i = 0; i < (int)n; i++) {
    ((char *)str)[i] = (unsigned char)c;
  }

  return str;
}

char *strchr(const char *stroc, int c) {
  const char *res = NULL;
  for (size_t i = 0; i <= strlen(stroc) && (res == NULL); i++) {
    if (stroc[i] == c) {
      res = stroc + i;
    }
  }
  return (char *)res;
}

size_t strcspn(const char* str1, const char* str2) {
  size_t length = 0;
  int i = 0;
  while (*(str1 + length)) {
    for (i = 0; *(str2 + i) && (*(str1 + length) != *(str2 + i)); i++)
      ;
    if (*(str1 + length) == *(str2 + i)) break;
    length++;
  }
  return length;
}

char *strncat(char *dest, const char *src, size_t n) {
  char *result = dest;

  while (*dest != '\0') {
    dest++;
  }

  while (*src != '\0' && n > 0) {
    *dest = *src;
    dest++;
    src++;
    n--;
  }

  *dest = '\0';

  return result;
}

int strncmp(const char* str1, const char* str2, size_t n) {
  if (n <= 0) return 0;
  int i = 0;

  while (i < (int)n && str1[i] && str2[i]) {
    if (str1[i] - str2[i]) {
      break;
    }
    i++;
  }

  return ((int)str1[i]) - ((int)str2[i]);
}

char *strncpy(char *dest, const char *src, size_t n) {
  int src_len = strlen(src);
  int change_len = src_len < (int)n ? (src_len + 1) : (int)n;

  for (int i = 0; i < change_len; i++) {
    ((char *)dest)[i] = ((char *)src)[i];
  }
  return dest;
}

char *strpbrk(const char *str, const char *sym) {
  const char *res = NULL;

  for (size_t i = 0; i < strlen(str) && res == NULL; i++) {
    const char *match = strchr(sym, str[i]);

    if (match != NULL) {
      res = str + i;
    }
  }
  return (char *)res;
}

char *strrchr(const char *stroc, int c) {
  const char *res = NULL;
  for (size_t i = 0; i <= strlen(stroc); i++) {
    if (stroc[i] == c) {
      res = stroc + i;
    }
  }
  return (char *)res;
}

char *strstr(const char *str_1, const char *str_2) {
  const char *res = NULL;
  while (*str_1 != '\0' && res == NULL) {
    if (*str_1 == *str_2 || *str_2 == 0) {
      const char *match_1 = str_1;
      const char *match_2 = str_2;
      while (*match_2 != '\0' && *match_1 == *match_2) {
        match_1++;
        match_2++;
      }
      if (*match_2 == '\0') {
        res = str_1;
      }
    }
    str_1++;
  }
  return (char *)res;
}

char *strtok(char *str, const char *delim) {
  static char *str_ptr = NULL;
  char *result = NULL;

  if (str != NULL) str_ptr = str;
  if (str_ptr == NULL || *str_ptr == '\0') return NULL;

  while (*str_ptr != '\0' && strchr(delim, *str_ptr) != NULL) {
    str_ptr++;
  }

  if (*str_ptr == '\0') return NULL;

  result = str_ptr;

  while (*str_ptr != '\0' && strchr(delim, *str_ptr) == NULL) {
    str_ptr++;
  }

  if (*str_ptr == '\0') {
    str_ptr = NULL;
  } else {
    *str_ptr = '\0';
    str_ptr++;
  }

  return result;
}
