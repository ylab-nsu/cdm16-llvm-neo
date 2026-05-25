// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

void
f (int x, int y)
{
  if (x % y != 0)
    abort ();
}

int
main ()
{
  f (-5, 5);
  exit (0);
}
