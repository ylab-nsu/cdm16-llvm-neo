#include "ctype.h"

int tolower(int c) {
  if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
  return c;
}

int toupper(int c) {
  if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
  return c;
}
