// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int
x(){signed char c=-1;return c<0;}

int
main(){if(x()!=1)abort();exit(0);}
