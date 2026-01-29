class CDMInst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  let Namespace = "CDM";

  let OutOperandList = outs;
  let InOperandList = ins;

  let AsmString = asmstr;
  let Pattern = pattern;
  let hasDelaySlot = 0;
}

class CDMInst16<dag outs, dag ins, string asmstr, list<dag> pattern>
  : CDMInst<outs, ins, asmstr, pattern> {
  bits<16> Inst;
  let Size = 2;
}

class CDMInst32<dag outs, dag ins, string asmstr, list<dag> pattern>
  : CDMInst<outs, ins, asmstr, pattern> {
  bits<32> Inst;
  let Size = 4;
}

class CDMPseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
    : CDMInst16<outs, ins, asmstr, pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
  let Inst = 0;
}

//===----------------------------------------------------------------------===//
// Instruction without operands: <|oooo|cxxx|xxxx|ffff|>
// o = opcode (5 bits)
// x = ignored bits (7 bits)
// f = secondary opcode (4 bits)
//===----------------------------------------------------------------------===//
class Inst0Op<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  let Inst{15 - 11} = 0b00000;
  let Inst{10 - 4} = 0b0000000;
  let Inst{3 - 0} = f;
}

//===----------------------------------------------------------------------===//
// 0op instruction with 16-bit immediate constant.
//===----------------------------------------------------------------------===//
class Inst0OpImm16<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern>: CDMInst32<outs, ins, asmstr, pattern> {
  bits<16> imm;

  let Inst{31 - 16} = imm;
  let Inst{15 - 11} = 0b00000;
  let Inst{10 - 4} = 0b0000000;
  let Inst{3 - 0} = f;
}

//===----------------------------------------------------------------------===//
// Instruction with 1 operand: <|ooox|xxxx|xfff|fddd|>
// o = opcode (3 bits)
// x = ignored bits (6 bits)
// f = secondary opcode (4 bits)
// d = first operand (3 bits)
//===----------------------------------------------------------------------===//
class Inst1Op<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> rd;

  let Inst{15 - 13} = 0b001;
  let Inst{12 - 7} = 0b000000;
  let Inst{6 - 3} = f;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// 1op instruction with 16-bit immediate constant.
//===----------------------------------------------------------------------===//
class Inst1OpImm16<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst32<outs, ins, asmstr, pattern> {
  bits<16> imm;
  bits<3> rd;

  let Inst{31 - 16} = imm;
  let Inst{15 - 13} = 0b001;
  let Inst{12 - 7} = 0b000000;
  let Inst{6 - 3} = f;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Instruction with 2 operands: <|oooo|cxff|ffrr|rddd|>
// o = opcode (5 bits)
// x = ignored bits (1 bit)
// f = secondary opcode (4 bits)
// r = source (3 bits)
// d = destination (3 bits)
//===----------------------------------------------------------------------===//
class Inst2Op<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> rs;
  bits<3> rd;

  let Inst{15 - 11} = 0b01000;
  let Inst{10} = 0b0;
  let Inst{9 - 6} = f;
  let Inst{5 - 3} = rs;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Memory instruction with 2 operands: <|oooo|cxff|ffrr|rddd|>
// o = opcode (5 bits)
// x = ignored bits (1 bit)
// f = secondary opcode (4 bits)
// r = source (3 bits)
// d = destination (3 bits)
//===----------------------------------------------------------------------===//
class InstMem2<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> rs;
  bits<3> rd;

  let Inst{15 - 11} = 0b01010;
  let Inst{10} = 0b0;
  let Inst{9 - 6} = f;
  let Inst{5 - 3} = rs;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Instruction with 1 operand and 1 immediate 6 bit value: <|ooof|fffK|KKKK|Kddd|>
// o = opcode (3 bits)
// f = secondary opcode (4 bits)
// K = constant data (6 bits)
// d = destination (3 bits)
//===----------------------------------------------------------------------===//
class InstImm6<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<6> imm;
  bits<3> rd;

  let Inst{15 - 13} = 0b011;
  let Inst{12 - 9} = f;
  let Inst{8 - 3} = imm;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Instruction with 1 operand and 1 immediate 9 bit value: <|ooof|fffK|KKKK|Kddd|>
// o = opcode (3 bits)
// f = secondary opcode (4 bits)
// K = constant data (9 bits)
//===----------------------------------------------------------------------===//
class InstImm9<bits<4> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<9> imm;

  let Inst{15 - 13} = 0b100;
  let Inst{12 - 9} = f;
  let Inst{8 - 0} = imm;
}

//===----------------------------------------------------------------------===//
// Instruction from mem3 group: <|oooo|fffr|rrss|sddd|>
// o = opcode (4 bits)
// f = secondary opcode (3 bits)
// r = first sourse (3 bits)
// s = second sourse (3 bits)
// d = destination(3 bits)
//===----------------------------------------------------------------------===//
class InstMem3< bits<3> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> rs1;
  bits<3> rs0;
  bits<3> rd;

  let Inst{15 - 12} = 0b1010;
  let Inst{11 - 9} = f;
  let Inst{8 - 6} = rs1;
  let Inst{5 - 3} = rs0;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Absolute branch: <|oooo|oxxx|xxxx|cccc|>
// o = opcode (5 bits)
// x = ignored bits (7 bits)
// c = condition (4 bits)
//===----------------------------------------------------------------------===//
class InstAbsBr<dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<4> c;

  let Inst{15 - 11} = 0b00001;
  let Inst{10 - 4} = 0b0000000;
  let Inst{3 - 0} = c;
}

//===----------------------------------------------------------------------===//
// Relative branch: <|oooc|cccK|KKKK|KKKK|>
// o = opcode (3 bits)
// c = condition (4 bits)
// K = constant data (9 bits)
//===----------------------------------------------------------------------===//
class InstRelBr<bits<3>opcode, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<4> c;
  bits<9> k;

  let Inst{15 - 13} = opcode;
  let Inst{12 - 9} = c;
  let Inst{8 - 0} = k;
}

//===----------------------------------------------------------------------===//
// Instruction shift: <|oooo|fffh|hhss|sddd|>
// o = opcode (4 bits)
// f = secondary opcode (3 bits)
// h = shift value (3 bits)
// s = source (3 bits)
// d = destination (3 bits)
//===----------------------------------------------------------------------===//
class InstShifts<bits<3> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> shamt;
  bits<3> rs;
  bits<3> rd;

  let Inst{15 - 12} = 0b0001;
  let Inst{11 - 9} = f;
  let Inst{8 - 6} = shamt;
  let Inst{5 - 3} = rs;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Instruction from group alu2: <|oooo|oxxf|ffss|sddd|>
// o = opcode (5 bits)
// x = ignored bits (2 bits)
// f = secondary opcode (3 bits)
// s = source (3 bits)
// d = destination (3 bits)
//===----------------------------------------------------------------------===//
class InstAlu2<bits<3> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> rs;
  bits<3> rd;

  let Inst{15 - 11} = 0b01011;
  let Inst{10 - 9} = 0b00;
  let Inst{8 - 6} = f;
  let Inst{5 - 3} = rs;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Instruction from group alu3: <|oooo|fffr|rrss|sddd|>
// o = opcode (4 bits)
// f = secondary opcode (3 bits)
// r = first souse (3 bits)
// s = second sourse (3 bits)
// d = destination (3 bits)
//===----------------------------------------------------------------------===//
class InstAlu3<bits<3> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> rs1;
  bits<3> rs0;
  bits<3> rd;

  let Inst{15 - 12} = 0b1011;
  let Inst{11 - 9} = f;
  let Inst{8 - 6} = rs1;
  let Inst{5 - 3} = rs0;
  let Inst{2 - 0} = rd;
}

//===----------------------------------------------------------------------===//
// Insdestructive instructions from alu3_ind: <|oooo|oxxf|ffss|sddd|>
// o = opcode (4 bits)
// x = ignored bits (2 bits)
// f = secondary opcode (3 bits)
// s = sourse (3 bits)
// d = destination (3 bits)
//===----------------------------------------------------------------------===//
class InstAlu3Ind<bits<3> f, dag outs, dag ins, string asmstr,
            list<dag> pattern> : CDMInst16<outs, ins, asmstr, pattern> {
  bits<3> rs;
  bits<3> rd;

  let Inst{15 - 11} = 0b01001;
  let Inst{10 - 9} = 0b00;
  let Inst{8 - 6} = f;
  let Inst{5 - 3} = rs;
  let Inst{2 - 0} = rd;
}
