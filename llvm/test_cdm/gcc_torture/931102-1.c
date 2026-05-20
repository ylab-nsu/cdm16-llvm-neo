// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

typedef union
{
  struct
    {
      char h, l;
    } b;
} T;

int
f (int x)
{
  int num = 0;
  T reg;

  reg.b.l = x;
  while ((reg.b.l & 1) == 0)
    {
      num++;
      reg.b.l >>= 1;
    }
  return num;
}

int
main ()
{
  if (f (2) != 1)
    abort ();
  exit (0);
}

