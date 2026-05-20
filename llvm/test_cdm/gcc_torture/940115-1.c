// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int
f (char *cp, char *end)
{
  return (cp < end);
}

int
main ()
{
  if (! f ((char *) 0, (char *) 1))
    abort();
  exit (0);
}
