// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int a = 1, b;

int
g () { return 0; }

void
h (x) {}

int
f ()
{
  if (g () == -1)
    return 0;
  a = g ();
  if (b >= 1)
    h (a);
  return 0;
}

int
main ()
{
  f ();
  if (a != 0)
    abort ();
  exit (0);
}
