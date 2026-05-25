// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

char
foo(char *bufp)
{
    int x = 80;
    return (*bufp++ = x ? 'a' : 'b');
}

int
main()
{
  char x;

  if (foo (&x) != 'a')
    abort ();

  exit (0);
}
