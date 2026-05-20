// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

typedef int t;

int
f(t y){switch(y){case 1:return 1;}return 0;}

int
main(){if(f((t)1)!=1)abort();exit(0);}
