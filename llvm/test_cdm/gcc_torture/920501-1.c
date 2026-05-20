// CHECK reg(r0) 0
/* { dg-require-effective-target untyped_assembly } */
/* { dg-additional-options "-std=gnu89" } */

void abort (void);
void exit (int);

int s[2];

int
x(){if(!s[0]){s[1+s[1]]=s[1];return 1;}}

int
main(){s[0]=s[1]=0;if(x(0)!=1)abort();exit(0);}
