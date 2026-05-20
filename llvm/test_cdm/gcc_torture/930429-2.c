// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int
f (int b)
{
  return (b >> 1) > 0;
}

int
main ()
{
  if (!f (9))
    abort ();
  if (f (-9))
    abort ();
  exit (0);
}
