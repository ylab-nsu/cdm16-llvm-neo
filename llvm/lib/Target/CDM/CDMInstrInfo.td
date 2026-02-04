include "CDMInstrFormats.td"

//===----------------------------------------------------------------------===//
// CDM Type Profiles
//===----------------------------------------------------------------------===//

def SDT_CDMRet : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def SDT_CDMCall : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;

// Extended shift node types.
def SDTIntShiftQuadOp : SDTypeProfile<4, 5, [
  SDTCisInt<0>, SDTCisSameAs<1, 0>,
  SDTCisSameAs<2, 0>, SDTCisSameAs<3, 0>,
  SDTCisSameAs<4, 0>, SDTCisSameAs<5, 0>,
  SDTCisSameAs<6, 0>, SDTCisSameAs<7, 0>, SDTCisInt<8>
]>;

def SDT_CDMCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;
def SDT_CDMCallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;

//===----------------------------------------------------------------------===//
// CDM SDNode definitions
//===----------------------------------------------------------------------===//

def CDMRet : SDNode<"CDMISD::Ret",
                    SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def CDMCall : SDNode<"CDMISD::Call", SDT_CDMCall,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

// Extended shift nodes
def shl_ext32 : SDNode<"CDMISD::SHL_EXT32", SDTIntShiftPairOp>;
def srl_ext32 : SDNode<"CDMISD::SRL_EXT32", SDTIntShiftPairOp>;
def sra_ext32 : SDNode<"CDMISD::SRA_EXT32", SDTIntShiftPairOp>;
def shl_ext64 : SDNode<"CDMISD::SHL_EXT64", SDTIntShiftQuadOp>;
def srl_ext64 : SDNode<"CDMISD::SRL_EXT64", SDTIntShiftQuadOp>;
def sra_ext64 : SDNode<"CDMISD::SRA_EXT64", SDTIntShiftQuadOp>;

// Load symbol
def load_sym : SDNode<"CDMISD::LOAD_SYM", SDTIntUnaryOp>;

// These are target-independent nodes, but have target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START",
                           SDT_CDMCallSeqStart, [SDNPHasChain, SDNPOutGlue]>;
def callseq_end
    : SDNode<"ISD::CALLSEQ_END",
             SDT_CDMCallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

//===----------------------------------------------------------------------===//
// CDM Operands, Complex Patterns and Transformations Definitions.
//===----------------------------------------------------------------------===//

// 9-bit relative JSR call target
def imm9CallTarget : Operand<i16> {
  let OperandType = "OPERAND_PCREL";
  let EncoderMethod = "encodeBranchTarget<CDM::fixup_call_imm9, 0>";
}

// 9-bit relative branch target
def imm9BrTarget : Operand<OtherVT> {
  let OperandType = "OPERAND_PCREL";
  let EncoderMethod = "encodeBranchTarget<CDM::fixup_branch_imm9, 0>";
}

// 9-bit stack offset for addsp
def imm9StackOffset : Operand<i16> {
  let EncoderMethod = "encodeStackOffset";
}

// 16-bit generic constant
def imm16 : Operand<i16> {
  let EncoderMethod = "encodeImm<FK_Data_2, 2>";
}

// 16-bit constant branch target
def imm16BrTarget : Operand<OtherVT> {
  let OperandType = "OPERAND_MEMORY";
  let EncoderMethod = "encodeBranchTarget<FK_Data_2, 2>";
}

// Condition code operand used in conditional branch instrcutions.
def condCode : Operand<OtherVT> {
  let PrintMethod = "printCondCode";
}

// reg address operand used in simple load/store
// instructions.
def memReg : Operand<iPTR> {
  let MIOperandInfo = (ops CPURegs);
  let OperandType = "OPERAND_MEMORY";
}

// reg+reg address operand used in load/store
// instructions with 3 operands.
def mem2Reg : Operand<iPTR> {
  let MIOperandInfo = (ops CPURegs, CPURegs);
  let OperandType = "OPERAND_MEMORY";
  let PrintMethod = "printMem2RegOperand";
}

// reg address pattern.
def addr : ComplexPattern<iPTR, 1, "SelectAddr", [], []>;

// reg+reg address pattern.
def addr2Reg : ComplexPattern<iPTR, 2, "SelectAddr2Reg", [], []>;

// Frame-relative address pattern.
def addrFrame : ComplexPattern<iPTR, 1, "SelectAddrFrameIndex", [frameindex], []>;

// Small immediate value between 1 and 8 used in shift instructions.
// Encoded as val - 1.
def shamt : Operand<i16> {
  let EncoderMethod = "encodeShamt";
}

// Immediate values with specific ranges (immM_N -> [M; N]).
def Imm1_8Leaf : ImmLeaf<i16, [{return (Imm >= 1) && (Imm <= 8);}]>;
def Imm9_16Leaf : ImmLeaf<i16, [{return (Imm >= 9) && (Imm <= 16);}]>;

def cdmizeCondXform : SDNodeXForm<cond, [{
  return CurDAG->getTargetConstant(CCToCondOp(N->get()), N, MVT::i16);
}]>;

//===----------------------------------------------------------------------===//
//===----------------------------------------------------------------------===//
// CDM Instruction list
//===----------------------------------------------------------------------===//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// 0op format
//===----------------------------------------------------------------------===//

// Invalid instruction
// Encoded as 0x0000.
def ZERO: Inst0Op<0b0000, (outs), (ins), "zero", []>;

// Halt processor
// Stops processor clock, changes processor status to HALTED.
def HALT: Inst0Op<0b0100, (outs), (ins), "halt", []>;

// Wait for interrupt
// Stops processor clock until an interrupt or an exception
// occurs, changes processor status to WAITING.
def WAIT: Inst0Op<0b0101, (outs), (ins), "wait", []>;

// Enable interrupts
// Sets I bit (bit 15) in PS thus enabling interrupts.
def EI: Inst0Op<0b0110, (outs), (ins), "ei", []>;

// Disable interrupts
// Clears I bit (bit 15) in PS thus disabling interrupts.
def DI: Inst0Op<0b0111, (outs), (ins), "di", []>;

// Jump to subroutine (imm16 absolute address)
// Pushes PC to stack and then does unconditional
// absolute branch to address imm16.
def JSRImm16 : Inst0OpImm16<0b1000, (outs), (ins imm16BrTarget:$imm), "jsr\t$imm", []> {
  let isCall = 1;
}

let isTerminator = 1, isReturn = 1, isBarrier = 1 in {
  // Generic return pseudo
  // Expands to:
  // - rts for normal subroutines
  // - rti for interrupt service routines
  def PseudoRet : CDMPseudo<(outs), (ins), "retconv", [(CDMRet)]>;

  // Return from interrupt
  // Pops a return address and a processor status word from the stack and loads
  // them into PC and PS respectively.
  def RTI : Inst0Op<0b1001, (outs), (ins), "rti", []>;

  // Return from subroutine
  // Pops a return address from the stack and loads it into PC.
  def RTS : Inst0Op<0b1011, (outs), (ins), "rts", []>;
}

// Push PC
// Pushes PC to the stack.
def PUPC : Inst0Op<0b1010, (outs), (ins), "pupc", []> {
  let Uses = [SP];
  let Defs = [SP];
}

// Pop PC
// Pops PC from the stack. Alias for rts.
def POPC : InstAlias<"popc", (RTS)>;

// Push SP
// Pushes SP to the stack.
def PUSP : Inst0Op<0b1100, (outs), (ins), "pusp", []> {
  let Uses = [SP];
  let Defs = [SP];
}

// Pop SP
// Pops SP from the stack.
def POSP : Inst0Op<0b1101, (outs), (ins), "posp", []> {
  let Uses = [SP];
  let Defs = [SP];
}

// Push PS
// Pushes PS to the stack.
def PUPS : Inst0Op<0b1110, (outs), (ins), "pups", []> {
  let Uses = [SP, PSR];
  let Defs = [SP];
}

// Pop PS
// Pops PS from the stack.
def POPS : Inst0Op<0b1111, (outs), (ins), "pops", []> {
  let Uses = [SP];
  let Defs = [SP, PSR];
}

//===----------------------------------------------------------------------===//
// 1op format
//===----------------------------------------------------------------------===//

let Uses = [SP], Defs = [SP] in {
  // Push register
  // Stores contents of rd to the top of the stack.
  def PUSH : Inst1Op<0b0000, (outs), (ins CPURegs:$rd), "push\t$rd", []> {
    let mayStore = 1;
  }

  // Pop register
  // Loads a value from the top of the stack to rd.
  def POP : Inst1Op<0b0001, (outs CPURegs:$rd), (ins), "pop\t$rd", []> {
    let mayLoad = 1;
  }

  // Add register to SP.
  // Adds value from rd to SP.
  def ADDSPReg : Inst1Op<0b1010, (outs), (ins CPURegs:$rd), "addsp\t$rd", []>;
}

// Load large immediate to register
// Loads immediate value imm16 to rd.
def LDIImm16 : Inst1OpImm16<0b0010, (outs CPURegs:$rd), (ins imm16:$imm), "ldi\t$rd, $imm", []> {
  let isReMaterializable = 1;
}
/*
 LD: LoadM32 -> LoadM -> FMem -> FL -> CpuoInst -> Instruction
 My LD: LoadM32 -> LoadM -> CDMInst
*/
def : Pattern<(i16(load_sym tglobaladdr:$addr)), [(LDIImm16 $addr)]>;
def : Pattern<(i16(load_sym tjumptable:$addr)), [(LDIImm16 $addr)]>;
def : Pat<(i16 imm:$imm), (LDIImm16 imm:$imm)>;

// Jump to subroutine by pointer
// Pushes PC to stack and then does unconditional
// absolute branch to address contained in rd.
def JSRR : Inst1Op<0b0011, (outs), (ins CPURegs:$rd), "jsrr\t$rd", []>{
  let isCall = 1;
}

// Load SP
// Copies value from SP to rd.
def LDSP : Inst1Op<0b0100, (outs CPURegs:$rd), (ins), "ldsp\t$rd", []> {
  let Uses = [SP];
}

// Store SP
// Copies value from rd to SP.
def STSP : Inst1Op<0b0101, (outs), (ins CPURegs:$rd), "stsp\t$rd", []> {
  let Defs = [SP];
}

// Load PS
// Copies value from PS to rd.
def LDPS : Inst1Op<0b0110, (outs CPURegs:$rd), (ins), "ldps\t$rd", []> {
  let Uses = [PSR];
}

// Store PS
// Copies value from rd to PS.
def STPS : Inst1Op<0b0111, (outs), (ins CPURegs:$rd), "stps\t$rd", []> {
  let Defs = [PSR];
}

// Load PC
// Copies value from PC to rd.
def LDPC : Inst1Op<0b1000, (outs CPURegs:$rd), (ins), "ldpc\t$rd", []>;

// Store PC
// Copies value from rd to PC. Performs an unconditional branch
// to absolute address stored in rd.
def STPC : Inst1Op<0b1001, (outs), (ins CPURegs:$rd), "stpc\t$rd", [(brind CPURegs:$rd)]> {
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
  let isIndirectBranch = 1;
}

//===----------------------------------------------------------------------===//
// 2op format
//===----------------------------------------------------------------------===//

// Copy register
// Copies value from rs to rd. Clobbers PS!
def MOVE : Inst2Op<0b0000, (outs CPURegs:$rd), (ins CPURegs:$rs), "move\t$rs, $rd", []>;

//===----------------------------------------------------------------------===//
// mem2/mem3 format
//===----------------------------------------------------------------------===//

multiclass CDMLoad<bits<3> opcode, string mnem, SDPatternOperator OpNode> {
    // reg addressing mode
    def "" : InstMem2<{0, opcode}, (outs CPURegs:$rd), (ins memReg:$rs),
              !strconcat(mnem, "\t$rs, $rd"),
              [(set CPURegs:$rd, (OpNode addr:$rs))]>;

    // reg+reg addressing mode
    def "2Reg" : InstMem3<opcode, (outs CPURegs:$rd), (ins (mem2Reg $rs0, $rs1):$regs),
              !strconcat(mnem, "\t$regs, $rd"),
              [(set CPURegs:$rd, (OpNode addr2Reg:$regs))]>;
}

multiclass CDMStore<bits<3> opcode, string mnem, SDPatternOperator OpNode> {
    // reg addressing mode
    def "" : InstMem2<{0, opcode}, (outs), (ins CPURegs:$rd, memReg:$rs),
              !strconcat(mnem, "\t$rs, $rd"),
              [(OpNode CPURegs:$rd, addr:$rs)]>;

    // reg+reg addressing mode
    def "2Reg" : InstMem3<opcode, (outs), (ins CPURegs:$rd, (mem2Reg $rs0, $rs1):$regs),
              !strconcat(mnem, "\t$regs, $rd"),
              [(OpNode CPURegs:$rd, addr2Reg:$regs)]>;
}

// Load word
// Loads 2 bytes from data memory pointed by rs to rd.
defm LDW : CDMLoad<0b000, "ldw", load>;

// Load unsigned byte
// Loads a byte from data memory pointed by rs to rd.
defm LDB : CDMLoad<0b001, "ldb", zextloadi8>;

// Load signed byte
// Loads a byte from data memory pointed by rs to rd with sext.
defm LDSB : CDMLoad<0b010, "ldsb", sextloadi8>;

// Load constant word
// Loads 2 bytes from program memory pointed by rs to rd.
defm LCW : CDMLoad<0b011, "lcw", null_frag>;

// Load constant unsigned byte
// Loads a byte from program memory pointed by rs to rd.
defm LCB : CDMLoad<0b100, "lcb", null_frag>;

// Load constant signed byte
// Loads a byte from program memory pointed by rs to rd with sext.
defm LCSB : CDMLoad<0b101, "lcsb", null_frag>;

// Store word
// Stores 2 bytes from rd to data memory pointed by rs.
defm STW : CDMStore<0b110, "stw", store>;

// Store byte
// Stores the least significant byte of rd to data memory pointed by rs.
defm STB : CDMStore<0b111, "stb", truncstorei8>;

// These patterns are needed for selecting "load anyext from i8"
def : Pat<(i16(extloadi8 addr2Reg:$addr)), (LDB2Reg addr2Reg:$addr)>;
def : Pat<(i16(extloadi8 addr:$addr)), (LDB addr:$addr)>;

//===----------------------------------------------------------------------===//
// imm6 format
//===----------------------------------------------------------------------===//

def immZext6 : ImmLeaf<i16, [{return (Imm >= -64) && (Imm <= 63);}]>;

def load_a : PatFrag<(ops node:$ptr), (load node:$ptr), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlign().value();
}]>;

def store_a  : PatFrag<(ops node:$val, node:$ptr), (store node:$val, node:$ptr), [{
  StoreSDNode *SD = cast<StoreSDNode>(N);
  return SD->getMemoryVT().getSizeInBits()/8 <= SD->getAlign().value();
}]>;

class LoadMStack<string instr_asm, PatFrag OpNode, Operand MemOperand,
                 ComplexPattern AddrPattern, bit Pseudo>
    : CDMInst16<(outs CPURegs:$ra), (ins MemOperand:$addr),
              !strconcat(instr_asm, "\t$ra, $addr"),
              [(set CPURegs:$ra, (OpNode AddrPattern:$addr))]> {
  let isPseudo = Pseudo;
}

class StoreMStack<string instr_asm, PatFrag OpNode, Operand MemOperand,
                  ComplexPattern AddrPattern, bit Pseudo>
    : CDMInst16<(outs), (ins CPURegs:$ra, MemOperand:$addr),
              !strconcat(instr_asm, "\t$ra, $addr"),
              [(OpNode CPURegs:$ra, AddrPattern:$addr)]> {
  let isPseudo = Pseudo;
}

let Uses = [FP] in {
  // stack words
  def lsw : LoadMStack<"lsw", load_a, memReg, addrFrame, 0>;
  def ssw : StoreMStack<"ssw", store_a, memReg, addrFrame, 0>;
  // stack bytes
  def ssb : StoreMStack<"ssb", truncstorei8, memReg, addrFrame, 0>;
  def lsb : LoadMStack<"lsb", zextloadi8, memReg, addrFrame, 0>;
  def lssb : LoadMStack<"lssb", sextloadi8, memReg, addrFrame, 0>;
}
def : Pat<(i16(extloadi8 addrFrame:$src)), (lsb addrFrame:$src)>;

let Defs = [PSR], isReMaterializable = 1, Constraints = "$rs = $rd" in {
  // Add immediate
  // Computes rd + imm6 and puts result in rd.
  def ADDImm6 : InstImm6<0b1100, (outs CPURegs:$rd), (ins CPURegs:$rs, i16imm:$imm), "add\t$rs, $imm",
                         [(set CPURegs:$rd, (add CPURegs:$rs, immZext6:$imm))]> {
    let PostEncoderMethod = "adjustImmOpCode<2, 9>";
  }

  // Subtract immediate
  // Computes rd - imm6 and puts result in rd.
  // Expands to add rd, -imm6
  def SUBImm6 : CDMPseudo<(outs CPURegs:$rd), (ins CPURegs:$rs, i16imm:$imm), "sub\t$rs, $imm", []> {
    let isCodeGenOnly = 0;
    let isAsmParserOnly = 1;
  }
}

// Compare with immediate
// Performs indestructible subtraction, sets flags
// and thus compares numbers rd and imm6.
def CMPImm6 : InstImm6<0b1110, (outs), (ins CPURegs:$rd, i16imm:$imm), "cmp\t$rd, $imm", []> {
  let Defs = [PSR];
  let PostEncoderMethod = "adjustImmOpCode<1, 9>";
}

// Test register
// Compares register with itself and sets Z, N flags according to the result. Alias for cmp rd, 0
def TST : InstAlias<"tst\t$rd", (CMPImm6 CPURegs:$rd, 0)>;

//===----------------------------------------------------------------------===//
// imm9 format
//===----------------------------------------------------------------------===//

// Add immediate to SP
// Adds imm bytes to SP.
def ADDSPImm9 : InstImm9<0b0100, (outs), (ins imm9StackOffset:$imm), "addsp\t$imm", []> {
  let Uses = [SP];
  let Defs = [SP];
  let PostEncoderMethod = "adjustImmOpCode<0, 9>";
}

// Jump to subroutine (imm9 offset)
// Pushes PC to stack and then does unconditional
// absolute branch to address PC + (imm * 2) + 2.
def JSRImm9 : InstImm9<0b0110, (outs), (ins imm9CallTarget:$imm), "jsr\t$imm", []> {
  let isCall = 1;
}
def : Pat<(CDMCall(i16 tglobaladdr:$target)), (JSRImm9 tglobaladdr:$target)>;
def : Pat<(CDMCall(i16 texternalsym:$target)), (JSRImm9 texternalsym:$target)>;

//===----------------------------------------------------------------------===//
// br_abs format
//===----------------------------------------------------------------------===//

// Absolute branch
// Loads imm16 to PC, passing control to the instruction located at the address imm16.
def BRImm16 : InstBrAbs<(outs), (ins imm16BrTarget:$imm), "br\t$imm", []> {
  let cond = 0xE;
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
}

// Absolute conditional branch
// Loads imm16 to PC if the specified condition is true.
def BCondImm16 : InstBrAbs<(outs), (ins condCode:$cond, imm16BrTarget:$imm),
                    "b$cond\t$imm", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

//===----------------------------------------------------------------------===//
// br_rel format
//===----------------------------------------------------------------------===//

// Absolute branch
// Loads imm16 to PC, passing control to the instruction located at the address imm16.
def BRImm9 : InstBrRel<0b110, (outs), (ins imm9BrTarget:$imm), "br\t$imm", [(br bb:$imm)]> {
  let cond = 0xE;
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
}

// Absolute conditional branch
// Loads imm16 to PC if the specified condition is true.
def BCondImm9 : InstBrRel<0b110, (outs), (ins condCode:$cond, imm9BrTarget:$imm),
                    "b$cond\t$imm", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

//===----------------------------------------------------------------------===//
// shifts format
//===----------------------------------------------------------------------===//

// Class for basic shift instructions.
multiclass Shift<bits<3> op, string instr_asm, SDNode OpNode> {
  let Defs = [PSR] in {
    // The shift amount is constant, and is between 1 and 8.
    def ""
        : InstShifts<op, (outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt),
                  !strconcat(instr_asm, "\t$rs, $rd, $shamt"),
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, Imm1_8Leaf:$shamt))]>;

    // The shift amount is constant, and is greater than 8.
    // Needs to be expanded into two "regular" (amount between 1 and 8) instructions.
    def _LARGEAMT
        : CDMPseudo<(outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt),
                  !strconcat(!strconcat(!strconcat("!", instr_asm), "_LARGEAMT"), "\t$rs, $rd, $shamt"),
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, Imm9_16Leaf:$shamt))]> {
      let usesCustomInserter = true;
    }

    // The shift amount is stored in a register.
    // Expanded into an inline loop.
    def _LOOP
        : CDMPseudo<(outs CPURegs:$rd), (ins CPURegs:$rs, CPURegs:$shamt),
                  !strconcat(!strconcat(!strconcat("!", instr_asm), "_VARAMT"), "\t$rs, $rd, $shamt"),
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, CPURegs:$shamt))]> {
      let usesCustomInserter = true;
    }
  }
}

// Class for 32-bit shift pseudos.
multiclass ShiftExt32<SDNode OpNode> {
  let Defs = [PSR] in {
    // The shift amount is stored in a register.
    // Expanded into an inline loop.
    def "_LOOP" : CDMPseudo<(outs CPURegs:$rdlo, CPURegs:$rdhi), (ins CPURegs:$rslo, CPURegs:$rshi, CPURegs:$shamt), "",
        [(set CPURegs:$rdlo, CPURegs:$rdhi, (OpNode CPURegs:$rslo, CPURegs:$rshi, CPURegs:$shamt))]> {
      let usesCustomInserter = true;
    }

    // The shift amount is always 1.
    // Expanded post-RA into a shift-rotate chain.
    def "" : CDMPseudo<(outs CPURegs:$rdlo, CPURegs:$rdhi), (ins CPURegs:$rslo, CPURegs:$rshi), "",
      [(set CPURegs:$rdlo, CPURegs:$rdhi, (OpNode CPURegs:$rslo, CPURegs:$rshi, 1))]> {
      let Constraints = "$rdlo = $rslo,$rdhi = $rshi,@earlyclobber $rdlo,@earlyclobber $rdhi";
    }
  }
}

// Class for 64-bit shift pseudos.
multiclass ShiftExt64<SDNode OpNode> {
  let Defs = [PSR] in {
    // The shift amount is stored in a register.
    // Expanded into an inline loop.
    def "_LOOP" : CDMPseudo<
      (outs CPURegs:$rd0, CPURegs:$rd1, CPURegs:$rd2, CPURegs:$rd3),
      (ins CPURegs:$rs0, CPURegs:$rs1, CPURegs:$rs2, CPURegs:$rs3, CPURegs:$shamt), "",
      [(set CPURegs:$rd0, CPURegs:$rd1, CPURegs:$rd2, CPURegs:$rd3,
        (OpNode CPURegs:$rs0, CPURegs:$rs1, CPURegs:$rs2, CPURegs:$rs3, CPURegs:$shamt))]> {
      let usesCustomInserter = true;
    }

    // The shift amount is always 1.
    // Expanded post-RA into a shift-rotate chain.
    def "" : CDMPseudo<
      (outs CPURegs:$rd0, CPURegs:$rd1, CPURegs:$rd2, CPURegs:$rd3),
      (ins CPURegs:$rs0, CPURegs:$rs1, CPURegs:$rs2, CPURegs:$rs3), "",
      [(set CPURegs:$rd0, CPURegs:$rd1, CPURegs:$rd2, CPURegs:$rd3,
        (OpNode CPURegs:$rs0, CPURegs:$rs1, CPURegs:$rs2, CPURegs:$rs3, 1))]> {
      let Constraints = "$rd0 = $rs0,$rd1 = $rs1,$rd2 = $rs2,$rd3 = $rs3,@earlyclobber $rd0,@earlyclobber $rd1,@earlyclobber $rd2,@earlyclobber $rd3";
    }
  }
}

// The outputs are tied to the corresponding inputs are prohibited from overlapping in 1-bit shifts,
// so that codegen doesn't try to swap the values at the same time as shifting them.

// Logic shift left
// Shifts left and fills with zeros.
defm SHL : Shift<0b000, "shl", shl>;

// Logic shift right
// Shifts right and fills with zeros.
defm SHR : Shift<0b001, "shr", srl>;

// Arithmetic shift right
// Shifts right and fills with the most
// significant bit of the original number.
defm SHRA : Shift<0b010, "shra", sra>;

// Cyclic left shift
// Shifts left, bit 15 shifts to bit 0.
defm ROL : Shift<0b011, "rol", rotl>;

// Cyclic right shift
// Shifts right, bit 0 shifts to bit 15.
defm ROR : Shift<0b010, "ror", rotr>;

// Cyclic left shift with carry
// Shifts left, bit 15 shifts to C, old C shifts to bit 0.
def RCL : InstShifts<0b101, (outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt), "rcl\t$rs, $rd, $shamt", []>;

// Cyclic right shift with carry
// Shifts right, bit 0 shifts to C, old C shifts to bit 15.
def RCR : InstShifts<0b110, (outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt), "rcr\t$rs, $rd, $shamt", []>;

// 32-bit and 64-bit pseudos.
// Expand to chains of 16-bit shifts and loops.
defm SHL_EXT32 : ShiftExt32<shl_ext32>;
defm SHR_EXT32 : ShiftExt32<srl_ext32>;
defm SHRA_EXT32 : ShiftExt32<sra_ext32>;
defm SHL_EXT64 : ShiftExt64<shl_ext64>;
defm SHR_EXT64 : ShiftExt64<srl_ext64>;
defm SHRA_EXT64 : ShiftExt64<sra_ext64>;

//===----------------------------------------------------------------------===//
// alu2 format
//===----------------------------------------------------------------------===//

let Defs = [PSR] in {
  // Negate
  // Computes 2's complement of rs and puts result in rd.
  def NEG : InstAlu2<0b000, (outs CPURegs:$rd), (ins CPURegs:$rs), "neg\t$rs, $rd", [(set CPURegs:$rd, (ineg CPURegs:$rs))]>;

  // Bitwise not
  // Computes 1's complement of rs and puts result in rd.
  def NOT : InstAlu2<0b001, (outs CPURegs:$rd), (ins CPURegs:$rs), "not\t$rs, $rd", [(set CPURegs:$rd, (not CPURegs:$rs))]>;

  // Sign extend
  // Fills higher 8 bits with 7-th bit of rs and puts result in rd.
  def SXT : InstAlu2<0b010, (outs CPURegs:$rd), (ins CPURegs:$rs), "sxt\t$rs, $rd", [(set CPURegs:$rd, (sext_inreg CPURegs:$rs, i8))]>;

  // Sign clear
  // Clears higher 8 bits of rs and puts result in rd.
  def SCL : InstAlu2<0b011, (outs CPURegs:$rd), (ins CPURegs:$rs), "scl\t$rs, $rd", []>;
}

//===----------------------------------------------------------------------===//
// alu3 format
//===----------------------------------------------------------------------===//

let Defs = [PSR], isCommutable = 1, isReMaterializable = 1 in {
  // Bitwise AND
  // Computes rs0 & rs1 and puts result in rd.
  def AND : InstAlu3<0b000, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "and\t$rs0, $rs1, $rd",
                     [(set CPURegs:$rd, (and CPURegs:$rs0, CPURegs:$rs1))]>;

  // Bitwise OR
  // Computes rs0 | rs1 and puts result in rd.
  def OR : InstAlu3<0b001, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "or\t$rs0, $rs1, $rd",
                    [(set CPURegs:$rd, (or CPURegs:$rs0, CPURegs:$rs1))]>;

  // Bitwise exclusive OR
  // Computes rs0 ^ rs1 and puts result in rd.
  def XOR : InstAlu3<0b010, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "xor\t$rs0, $rs1, $rd",
                     [(set CPURegs:$rd, (xor CPURegs:$rs0, CPURegs:$rs1))]>;

  // Add
  // Computes rs0 + rs1 and puts result in rd.
  def ADDReg : InstAlu3<0b100, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "add\t$rs0, $rs1, $rd",
                        [(set CPURegs:$rd, (add CPURegs:$rs0, CPURegs:$rs1))]>;

  // Add with carry
  // Computes rs0 + rs1 + C and puts result in rd.
  def ADDC : InstAlu3<0b101, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "addc\t$rs0, $rs1, $rd", []>;
}

let Defs = [PSR], isCommutable = 0 in {
  // Bit clear
  // Clears bits in rs0 that are set in rs1.
  // Computes rs0 & (~rs1) and puts result in rd.
  def BIC : InstAlu3<0b011, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "bic\t$rs0, $rs1, $rd",
                     [(set CPURegs:$rd, (and CPURegs:$rs0, (not CPURegs:$rs1)))]>;

  // Subtract
  // Computes rs0 - rs1 and puts result in rd.
  def SUB : InstAlu3<0b110, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "sub\t$rs0, $rs1, $rd",
                     [(set CPURegs:$rd, (sub CPURegs:$rs0, CPURegs:$rs1))]>;

  // Subtract with carry
  // Computes rs0 - rs1 + C - 1 and puts result in rd.
  def SUBC : InstAlu3<0b111, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "subc\t$rs0, $rs1, $rd", []>;
}

// Bit set
// Sets bits in rs0 that are set in rs1. Alias for OR.
def BIS : InstAlias<"bis\t$rs0, $rs1, $rd", (OR CPURegs:$rd, CPURegs:$rs0, CPURegs:$rs1)>;

// TODO why is this a separate instruction?
def load_frame_address
    : InstAlu3<0b100, (outs CPURegs:$rd), (ins CPURegs:$rs0), "add\t$rs0, fp, $rd", []> {
  let rs1 = 7;
  let isCodeGenOnly = 1;
  let Uses = [FP];
}
def : Pat<(i16 addrFrame:$addr), (load_frame_address(LDIImm16 CPURegs:$addr))>;

//===----------------------------------------------------------------------===//
// alu3_ind format
//===----------------------------------------------------------------------===//

let Defs = [PSR] in {
  // Compare
  // Performs indestructible subtraction, sets PS
  // and thus compares numbers in registers rs and rd.
  def CMPReg : InstAlu3Ind<0b000, (outs), (ins CPURegs:$rs, CPURegs:$rd), "cmp\t$rs, $rd", []>;

  // Bit test
  // Performs indestructible AND and sets PS. If any of bits that
  // are set in bitmask rd are also set in rs, then Z is 0, otherwise Z is 1.
  def BIT : InstAlu3Ind<0b110, (outs), (ins CPURegs:$rs, CPURegs:$rd), "bit\t$rs, $rd", []>;
}

//===----------------------------------------------------------------------===//
// Branch/select pseudos
//===----------------------------------------------------------------------===//

def PseudoBCondRR
    : CDMPseudo<(outs), (ins condCode:$cnd, CPURegs:$lhs, CPURegs:$rhs, imm16BrTarget:$target), "bcondrr", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

def PseudoBCondRI
    : CDMPseudo<(outs), (ins condCode:$cnd, CPURegs:$lhs, i16imm:$rhs, imm16BrTarget:$target), "bcondri", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

def PseudoSelectCC
    : CDMPseudo<(outs CPURegs:$dst),
                (ins CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, condCode:$cnd), "selectcc", []> {
  let usesCustomInserter = true;
}

def PseudoSelectCCI
    : CDMPseudo<(outs CPURegs:$dst),
                (ins CPURegs:$lhs, i16imm:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, condCode:$cnd), "selectcci", []> {
  let usesCustomInserter = true;
}
def : Pat<(brcc cond:$cnd, CPURegs:$lhs, immZext6:$rhs, bb:$target),
          (PseudoBCondRI (cdmizeCondXform $cnd), CPURegs:$lhs, immZext6:$rhs, imm16BrTarget:$target)>;

def : Pat<(brcc cond:$cnd, CPURegs:$lhs, CPURegs:$rhs, bb:$target),
          (PseudoBCondRR (cdmizeCondXform $cnd), CPURegs:$lhs, CPURegs:$rhs, imm16BrTarget:$target)>;

def : Pat<(selectcc CPURegs:$lhs, immZext6:$rhs, CPURegs:$vtrue, CPURegs:$vfalse,
              cond:$cnd),
          (PseudoSelectCCI CPURegs:$lhs, immZext6:$rhs, CPURegs:$vtrue,
              CPURegs:$vfalse, (cdmizeCondXform $cnd))>;

def : Pat<(selectcc CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue, CPURegs:$vfalse,
              cond:$cnd),
          (PseudoSelectCC CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue,
              CPURegs:$vfalse, (cdmizeCondXform $cnd))>;

let Defs = [SP], Uses = [SP] in {
  def ADJCALLSTACKDOWN
      : CDMPseudo<(outs), (ins imm16:$amt1, imm16:$amt2),
                  "!ADJCALLSTACKDOWN $amt1", [(callseq_start timm:$amt1, timm:$amt2)]>;
  def ADJCALLSTACKUP
      : CDMPseudo<(outs), (ins imm16:$amt1, imm16:$amt2),
                  "!ADJCALLSTACKUP $amt1", [(callseq_end timm:$amt1, timm:$amt2)]>;
}
