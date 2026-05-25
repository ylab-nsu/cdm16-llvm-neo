// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

void
f (unsigned n)
{
  if ((int) n >= 0)
    abort ();
}

int
main ()
{
  unsigned x = ~0;
  f (x);
  exit (0);
}
