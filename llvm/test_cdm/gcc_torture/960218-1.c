// CHECK reg(r0) 0
/* { dg-additional-options "-fpermissive" } */

void abort (void);
void exit (int);

int gl;

int
g (int x)
{
  gl = x;
  return 0;
}

int
f (int x)
{
  int a = ~x;
  while (a)
    a = g (a);
}

int
main ()
{
  f (3);
  if (gl != -4)
    abort ();
  exit (0);
}
