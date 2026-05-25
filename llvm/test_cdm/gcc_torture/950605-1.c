// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

void
f (unsigned char c)
{
  if (c != 0xFF)
    abort ();
}

int
main ()
{
  f (-1);
  exit (0);
}
