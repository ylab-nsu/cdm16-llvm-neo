__attribute__((noinline))
int foo(int a) {
    if (a & 1) {
        return a;
    }
    return 0;
}

int main(){
	return foo(1);
}
