// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int
f (int x)
{
  if (x != 0 || x == 0)
    return 0;
  return 1;
}

int
main ()
{
  if (f (3))
    abort ();
  exit (0);
}
