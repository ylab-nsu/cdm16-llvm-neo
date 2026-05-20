// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int
f(int x){int i;for(i=0;i<8&&(x&1)==0;x>>=1,i++);return i;}

int
main(){if(f(4)!=2)abort();exit(0);}
