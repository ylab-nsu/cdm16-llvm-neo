// CHECK reg(r0) 0
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int
x(const char*s){char a[1];const char*ss=s;a[*s++]|=1;return(int)ss+1==(int)s;}

int
main(){if(x("")!=1)abort();exit(0);}
