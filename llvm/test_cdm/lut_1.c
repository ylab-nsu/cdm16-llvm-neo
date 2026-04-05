// CHECK reg(r0) -1

__attribute__((noinline))
int lut(int i){

  switch (i) {
  case 1:
    return i+0xf100;
    break;
  case 2:
    return i+0xf200;;
  case 4:
    return i+0xf400;;
    break;
  case 5:
    return i+0xf500;
  default:
    return -1;
  }
}

int main(){
  return lut(-10);
}
