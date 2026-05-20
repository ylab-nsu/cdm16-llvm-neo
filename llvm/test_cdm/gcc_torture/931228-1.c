// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int
f (int x)
{
  x &= 010000;
  x &= 007777;
  x ^= 017777;
  x &= 017770;
  return x;
}

int
main ()
{
  if (f (-1) != 017770)
    abort ();
  exit (0);
}
