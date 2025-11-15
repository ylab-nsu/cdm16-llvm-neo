__attribute__((noinline))
int sum(int *array, int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        result += array[i];
    }
    return result;
}

int size = 5;

int main(void) {
    int array[size];
    array[0] = 13;
    array[1] = 17;
    array[2] = 5;
    array[3] = 7;
    array[4] = 3;
    return sum(array, size);
}
