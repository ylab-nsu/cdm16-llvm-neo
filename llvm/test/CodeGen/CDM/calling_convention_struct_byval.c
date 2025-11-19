// RUN: clang -target cdm -O1 -S -o /dev/stdout %s | FileCheck %s

// Test for struct by val passing:
// - Caller allocates struct on stack
// - Caller passes pointer to this struct as argument

struct s {
	int a;
	int b;
};

__attribute__((noinline))
int foo(struct s ss){
// CHECK-LABEL: foo>
// CHECK-NEXT: # %bb.0:
// CHECK-NEXT: push fp
// CHECK-NEXT: ldsp fp
// CHECK-NEXT: ldi r1, 2
// CHECK-NEXT: ldw r0, r1, r1
// CHECK-NEXT: ldw r0, r0
// CHECK-NEXT: add r1, r0, r0
// CHECK-NEXT: stsp fp
// CHECK-NEXT: pop fp
// CHECK-NEXT: rts
	return ss.a + ss.b;
}

int main(){
// CHECK-LABEL: main>
//  CHECK-NEXT: # %bb.0:
//  CHECK-NEXT: push	fp
//  CHECK-NEXT: ldsp	fp
//  CHECK-NEXT: addsp	-8
//  
//  CHECK-NEXT: ldi	r0, -4
//  CHECK-NEXT: add	r0, fp, r0
//  CHECK-NEXT: ldi	r1, 2
//  CHECK-NEXT: ldi	r2, 3
//  CHECK-NEXT: stw	r0, r1, r2
//  CHECK-NEXT: ldi	r0, -8
//  CHECK-NEXT: add	r0, fp, r0
//  CHECK-NEXT: stw	r0, r1, r2
//  CHECK-NEXT: ssw	r1, -4
//  CHECK-NEXT: ssw	r1, -8
//
//  CHECK-NEXT: addsp	-8
//  CHECK-NEXT: jsr	foo
//  CHECK-NEXT: addsp	8
//
//  CHECK-NEXT: addsp	8
//  CHECK-NEXT: stsp	fp
//  CHECK-NEXT: pop	fp
//  CHECK-NEXT: rts
	struct s ss;
	ss.a = 2;
	ss.b = 3;

	return foo(ss);
}

