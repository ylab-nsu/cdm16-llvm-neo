// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

short
f(short *p)
{
  short x = *p;
  return (--x < 0);
}

int
main()
{
  short x = -10;
  if (!f(&x))
    abort();
  exit(0);
}
