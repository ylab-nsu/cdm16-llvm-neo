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
// CHECK-NEXT: ldi [[REG1:r[0-6]]], 2
// CHECK-NEXT: ldw r0, [[REG1]], [[REG2:r[0-6]]]
// CHECK-NEXT: ldw r0, [[REG3:r[0-6]]]
// CHECK-NEXT: add [[REG2]], [[REG3]], r0
// CHECK-NEXT: stsp fp
// CHECK-NEXT: pop fp
// CHECK-NEXT: rts
	return ss.a + ss.b;
}

int main(){
// CHECK-LABEL: main>
//  
//  CHECK-DAG: ldi [[RSD:r[0-6]]], 2
//  CHECK-DAG: ldi [[RS2:r[0-6]]], 3
//
//  CHECK: ldi r0, -8
//  CHECK-NEXT: add r0, fp, r0
//  CHECK: stw r0, [[RSD]], [[RS2]]
//  CHECK: ssw [[RSD]], -8
//
//  CHECK-NEXT: addsp	-8
//  CHECK-NEXT: jsr	foo
//  CHECK-NEXT: addsp	8
	struct s ss;
	ss.a = 2;
	ss.b = 3;

	return foo(ss);
}

