// CHECK reg(r0) 0
/* { dg-additional-options "-fpermissive" } */

void abort (void);
void exit (int);

int
wwrite(long long i)
{
  switch(i)
    {
    case 3:
    case 10:
    case 23:
    case 28:
    case 47:
      return 0;
    default:
      return 123;
    }
}

int
main()
{
  if (wwrite((long long) 0) != 123)
    abort();
  exit(0);
}

