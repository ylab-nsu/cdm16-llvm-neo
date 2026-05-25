// CHECK reg(r0) 0
/* { dg-additional-options "-fpermissive" } */

void abort (void);
void exit (int);

unsigned int a[0x1000];
extern const unsigned long v;
const unsigned long v = 0xdeadbeefL;

void
f (a)
     unsigned long a;
{
  if (a != 0xdeadbeefL)
    abort();
}

int
main ()
{
  f (v);
  f (v);
  exit (0);
}
