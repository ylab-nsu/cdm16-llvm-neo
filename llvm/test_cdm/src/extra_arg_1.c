__attribute__((noinline))
int foo(int a, int b, int c, int d, int e){
	return a + b + c + d + e;
}

int main(){
	return foo(1, 2, 3, 4, 5);
}
