__attribute__((noinline))
int foo(int a, int b, int c, int d){
	if (((a == b) == c) == d){
		return a + b + c + d;
	}
	return 0;
}

int main(){
	return foo(2, 2, 1, 1);
}
