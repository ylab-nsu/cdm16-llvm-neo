// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

#include <limits.h>

void abort (void);
void exit (int);

int n = 0;

void
g (int i)
{
  n++;
}

void
f (int m)
{
  int i;
  i = m;
  do
    {
      g ((int)((unsigned)i * INT_MAX) / 2);
    }
  while (--i > 0);
}

int
main ()
{
  f (4);
  if (n != 4)
    abort ();
  exit (0);
}
