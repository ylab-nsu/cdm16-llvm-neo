target datalayout = "e-S16-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-m:C-n16"

; RUN: llc -mtriple=cdm < %s | FileCheck %s

; Test for accessing data on stack frame located too far from frame pointer to use lsw/ssw/lsb/ssb

; Function Attrs: nofree noinline norecurse nounwind memory(argmem: readwrite, inaccessiblemem: readwrite)
define i16 @foo(ptr %arr) #0 {
entry:
; CHECK-LABEL: foo>
; CHECK-NOT: lsw {{r[0-6]}}, {{-[0-9]+}}
; CHECK: ldi [[REG:r[0-6]]], [[OFF:-[0-9]+]]
; CHECK-NEXT: ldw [[REG]], fp, {{r[0-6]}}
  %lol = alloca [228 x i16], align 2
  %0 = load volatile i16, ptr %lol, align 2
  ret i16 %0
}

attributes #0 = { nofree noinline norecurse nounwind memory(argmem: readwrite, inaccessiblemem: readwrite) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
