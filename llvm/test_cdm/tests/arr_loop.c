// CHECK reg(r0) 3

unsigned char field[32][32];

int main(){
  for(int i = 0; i < 32; i++){
    for(int j = 0; j < 32; j++){
      field[i][j] = i+j;
    }
  }
  return (int)(field[1][2]);
}
