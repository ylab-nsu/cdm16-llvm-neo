int lol_size = 228;

__attribute__((noinline)) 
int foo(volatile int arr[90]) {
  volatile int lol[228];
  int res = arr[0] ^ arr[1];
  arr[2] &= arr[3];
  arr[6] -= arr[5];
  arr[4] += arr[0] - arr[2] + arr[6];
  res |= arr[4];
  return res;
}

int size = 90;

int main() {
  volatile int arr[size];
  arr[0] = 69;
  arr[1] = 420;
  arr[2] = 52;
  arr[3] = 42;
  arr[4] = 1337;
  arr[5] = 228;
  arr[6] = 1488;
  return foo(arr) - 6;
}
