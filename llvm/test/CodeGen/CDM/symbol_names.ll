target datalayout = "e-m:e-p:16:16-i8:8-i16:16-i32:16-i64:16-f16:16-f32:16-f64:16-f128:16-n16-S16"

; RUN: llc -mtriple=cdm-cocas < %s | FileCheck %s

; Test label name correction

; CHECK-LABEL: main>
define void @main() #0 {
    ret void
}

; CHECK-LABEL: a___24___main___24___b>
define void @a$main$b() #0 {
    ret void
}

; CHECK-LABEL: .foo>
define void @".foo"() #0 {
    ret void
}

; CHECK-LABEL: a.foo>
define void @"a.foo"() #0 {
    ret void
}

; CHECK-LABEL: bar.>
define void @"bar."() #0 {
    ret void
}

; CHECK-LABEL: ___30___.>
define void @"0."() #0 {
    ret void
}

; CHECK-LABEL: ___30___foo.>
define void @"0foo."() #0 {
    ret void
}

; CHECK-LABEL: ___30___0abc>
define void @"00abc"() #0 {
    ret void
}

; CHECK-LABEL: a0b1>
define void @"a0b1"() #0 {
    ret void
}

; CHECK-LABEL: ___25______3C____hehe____3E___>
define void @"%<_hehe_>"() #0 {
    ret void
}

; CHECK-LABEL: ___D0______BF______D1______80______D0______B8______D0______B2______D0______B5______D1______82___>
define void @"привет"() #0 {
    ret void
}
