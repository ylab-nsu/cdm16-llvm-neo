// CHECK reg(r0) 3045

__attribute__((noinline)) 
int foo(volatile int arr[90]) {
  volatile int lol[228] = {0};
  int res = arr[0] ^ arr[1];
  arr[2] &= arr[3];
  arr[6] -= arr[5];
  arr[4] += arr[0] - arr[2] + arr[6];
  res |= arr[4];
  return res;
}

int main() {
  volatile int arr[90] = {69, 420, 52, 42, 1337, 228, 1488};
  return foo(arr) - 6;
}
