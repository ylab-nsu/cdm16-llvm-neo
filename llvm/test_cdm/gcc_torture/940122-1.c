// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

char *a = 0;
char *b = 0;

void
g (int x)
{
  if ((!!a) != (!!b))
    abort ();
}

void
f (int x)
{
  g (x * x);
}

int
main ()
{
  f (100);
  exit (0);
}
