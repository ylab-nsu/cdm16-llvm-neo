// CHECK reg(r0) 0
/* { dg-additional-options "-fpermissive" } */

void abort (void);
void exit (int);

signed long long int
f (signed long long int x)
{
  return x > 0xFFFFFFFFLL || x < -0x80000000LL;
}

int
main ()
{
  if (f (0) != 0)
    abort ();
  exit (0);
}
