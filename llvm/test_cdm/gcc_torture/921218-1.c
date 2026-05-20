// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

unsigned char
f()
{
  return (unsigned char)("\377"[0]);
}

int
main()
{
  if (f() != (unsigned char)(0377))
    abort();
  exit (0);
}
