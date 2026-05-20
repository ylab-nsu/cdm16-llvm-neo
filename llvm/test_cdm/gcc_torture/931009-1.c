// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

static void
g (int *out, int size, int lo, int hi)
{
  int j;

  for (j = 0; j < size; j++)
    out[j] = j * (hi - lo);
}


void
f ()
{
  int a[2];

  g (a, 2, 0, 1);

  if (a[0] != 0 || a[1] != 1)
    abort ();
}

int
main ()
{
  f ();
  exit (0);
}
