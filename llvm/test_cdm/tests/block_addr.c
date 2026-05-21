// CHECK reg(r0) 16

__attribute__((noinline)) int add(int value, int count) {
    static const void *labels[] = {
        &&add0, &&add1, &&add2, &&add3, &&add4, &&add5,
    };
    goto *labels[count];
add5:
    value++;
add4:
    value++;
add3:
    value++;
add2:
    value++;
add1:
    value++;
add0:
    return value;
}

int main(){
  return add(10, 3) + add(1, 2);
}
