// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

#include <stdarg.h>

void exit (int);

int
f(int m)
{
  int i,s=0;
  for(i=0;i<m;i++)
    s+=i;
  return s;
}

int
main()
{
  exit (0);
}
