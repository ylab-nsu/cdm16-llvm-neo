__attribute__((noinline))
int foo(int a, int b, int c, int d){
	return a + b - c;
}

__attribute__((noinline))
int bar(int a, int b, int c, int d){
	if (foo(d, c, b, a)){
		return 1;
	}
	return 228;
}

int main(){
	return bar(0, 4, 2, 2);
}
