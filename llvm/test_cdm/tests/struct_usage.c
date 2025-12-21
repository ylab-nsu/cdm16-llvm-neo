// CHECK reg(r0) 733

typedef struct {
  int X;
  int Y;
  int Z;
  char Label[10];
} LabeledVecT;

__attribute__((noinline)) void labeledVecClearLabel(LabeledVecT *volatile Vec) {
  Vec->Label[0] = '\0';
}

__attribute__((noinline)) __attribute__((optnone)) void
labeledVecUseless(LabeledVecT Vec) {
  Vec.X = 1337;
}

__attribute__((noinline)) int labeledVecElementsSum(const LabeledVecT Vec) {
  return Vec.X + Vec.Y + Vec.Z;
}

int main(void) {
  LabeledVecT Vec = {.Label = "veloc", .X = 1, .Y = 42, .Z = 690};
  labeledVecClearLabel(&Vec);

  labeledVecUseless(Vec);

  if (Vec.Label[0] == '\0') // cleared
    return labeledVecElementsSum(Vec);

  return 0;
}
