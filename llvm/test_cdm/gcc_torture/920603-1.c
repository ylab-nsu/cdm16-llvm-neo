// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

void
f(got){if(got!=0xffff)abort();}

int
main(){signed char c=-1;unsigned u=(unsigned short)c;f(u);exit(0);}
