// CHECK reg(r0) 0

// vibecoded as fuck

#ifdef __unix__
#include <stdio.h>
#endif

#define ANS 42556

unsigned short force_register_pressure() {
  volatile unsigned short array[200];

  for (unsigned i = 0; i < 200; i++) {
    array[i] = i;
  }

  unsigned short v0 = array[0];
  unsigned short v1 = array[1];
  unsigned short v2 = array[2];
  unsigned short v3 = array[3];
  unsigned short v4 = array[4];
  unsigned short v5 = array[5];
  unsigned short v6 = array[6];
  unsigned short v7 = array[7];
  unsigned short v8 = array[8];
  unsigned short v9 = array[9];
  unsigned short v10 = array[10];
  unsigned short v11 = array[11];
  unsigned short v12 = array[12];
  unsigned short v13 = array[13];
  unsigned short v14 = array[14];
  unsigned short v15 = array[15];

  for (unsigned i = 0; i < 10; i++) {
    v0 = v0 + v1;
    v1 = v1 + v2;
    v2 = v2 + v3;
    v3 = v3 + v4;
    v4 = v4 + v5;
    v5 = v5 + v6;
    v6 = v6 + v7;
    v7 = v7 + v8;
    v8 = v8 + v9;

    v9 = v9 + v10;
    v10 = v10 + v11;
    v11 = v11 + v12;
    v12 = v12 + v13;
    v13 = v13 + v14;
    v14 = v14 + v15;
    v15 = v15 + v0;

    array[i] = array[i] + v0;
    array[128] = v0;
  }

  // force all variables to be used to prevent optimization
  array[0] = v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9 + v10 + v11 + v12 +
             v13 + v14 + v15;

  unsigned short acc = 0;
  for (unsigned i = 0; i < 200; i++) {
    acc += array[i];
  }

  return acc;
}

int main() {
  unsigned ans = force_register_pressure();

#ifdef __unix__
  printf("%hu\n", ans);
#endif

  return ans == ANS ? 0 : 1;
}
