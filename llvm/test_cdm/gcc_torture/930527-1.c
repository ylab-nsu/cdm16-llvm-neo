// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

unsigned char
f (unsigned char x)
{
  return (0x50 | (x >> 4)) ^ 0xff;
}

int
main ()
{
  if (f (0) != 0xaf)
    abort ();
  exit (0);
}
