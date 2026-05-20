// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

static inline unsigned short fu (unsigned short data)
{
  return data;
}

void
ru(unsigned short i)
{
   if(fu(i++)!=5)abort();
   if(fu(++i)!=7)abort();
}

static inline signed short fs (signed short data)
{
  return data;
}

void
rs(signed short i)
{
   if(fs(i++)!=5)abort();
   if(fs(++i)!=7)abort();
}

int
main()
{
  ru(5);
  rs(5);
  exit(0);
}
