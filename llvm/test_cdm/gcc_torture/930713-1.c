// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

typedef struct
{
  char x;
} T;

T
f (T s1)
{
  T s1a;
  s1a.x = 17;
  return s1a;
}

int
main ()
{
  T s1a, s1b;
  s1a.x = 13;
  s1b = f (s1a);
  if (s1a.x != 13 || s1b.x != 17)
    abort ();
  exit (0);
}
