include "CDMInstrFormats.td"

//===----------------------------------------------------------------------===//
// CDM Type Profiles
//===----------------------------------------------------------------------===//

def SDT_CDMRet : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def SDT_CDMCall : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;

def SDT_CDMCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;
def SDT_CDMCallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;

def SDTIntShiftQuadOp : SDTypeProfile<4, 5, [
  SDTCisInt<0>, SDTCisSameAs<1, 0>,
  SDTCisSameAs<2, 0>, SDTCisSameAs<3, 0>,
  SDTCisSameAs<4, 0>, SDTCisSameAs<5, 0>,
  SDTCisSameAs<6, 0>, SDTCisSameAs<7, 0>, SDTCisInt<8>
]>;

//===----------------------------------------------------------------------===//
// CDM SDNode definitions
//===----------------------------------------------------------------------===//

// Return from subroutine node.
def cdm_rts : SDNode<"CDMISD::RTS",
                    SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

// Return from interrupt node.
def cdm_rti : SDNode<"CDMISD::RTI",
                    SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

// Call subroutine node.
def cdm_call : SDNode<"CDMISD::CALL", SDT_CDMCall,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

// The beginning and end of a call sequence. Needed for stack adjustment.
// These are target-independent nodes, but have target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START",
                           SDT_CDMCallSeqStart, [SDNPHasChain, SDNPOutGlue]>;
def callseq_end : SDNode<"ISD::CALLSEQ_END",
                         SDT_CDMCallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

// Wrapper node for global addresses, jump table indices and such.
// Transformed into LDI instructions.
def load_sym : SDNode<"CDMISD::LOAD_SYM", SDTIntUnaryOp>;

// Extended bit shift nodes,
def shl_ext32 : SDNode<"CDMISD::SHL_EXT32", SDTIntShiftPairOp>;
def srl_ext32 : SDNode<"CDMISD::SRL_EXT32", SDTIntShiftPairOp>;
def sra_ext32 : SDNode<"CDMISD::SRA_EXT32", SDTIntShiftPairOp>;
def shl_ext64 : SDNode<"CDMISD::SHL_EXT64", SDTIntShiftQuadOp>;
def srl_ext64 : SDNode<"CDMISD::SRL_EXT64", SDTIntShiftQuadOp>;
def sra_ext64 : SDNode<"CDMISD::SRA_EXT64", SDTIntShiftQuadOp>;

//===----------------------------------------------------------------------===//
// CDM assembly operand classes
//===----------------------------------------------------------------------===//

// The sign bit is not part of the encoding of imm6 and imm9 fields in most
// instructions so they are effectively 7-bit and 10-bit respectively.

// Allows signed and unsigned 16-bit values and symbols
def immSymAsmOp : AsmOperandClass {
  let Name = "ImmSym";
  let PredicateMethod = "isImmN<16, 0, true, true, true>";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Allows signed 10-bit values known at assembly time
def imm9AsmOp : AsmOperandClass {
  let Name = "Imm9";
  let PredicateMethod = "isImmN<10, 0, true, false>";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseSmallImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Allows signed 10-bit values known at assembly time
// Must be even, but the range is doubled
def imm9EvenAsmOp : AsmOperandClass {
  let Name = "Imm9Even";
  let PredicateMethod = "isImmN<10, 1, true, false>";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseSmallImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Allows signed 7-bit values known at assembly
def imm6AsmOp : AsmOperandClass {
  let Name = "Imm6";
  let PredicateMethod = "isImmN<7, 0, true, false>";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseSmallImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Allows signed 7-bit values known at assembly
// (positive/negative ranges are swapped)
def imm6SubAsmOp : AsmOperandClass {
  let Name = "Imm6Sub";
  let PredicateMethod = "isImm6Sub";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseSmallImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Allows signed 7-bit values known at assembly time
// Must be even, but the range is doubled
def imm6EvenAsmOp : AsmOperandClass {
  let Name = "Imm6Even";
  let PredicateMethod = "isImmN<7, 1, true, false>";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseSmallImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Allows values between 0 and 255 (inclusive)
def interruptVecAsmOp : AsmOperandClass {
  let Name = "InterruptVec";
  let PredicateMethod = "isImmN<9, 0, false, true>";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseSmallImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Allows values between 1 and 8 (inclusive)
def shamtAsmOp : AsmOperandClass {
  let Name = "Shamt";
  let PredicateMethod = "isShamt";
  let RenderMethod = "addImmOperands";
  let ParserMethod = "parseSmallImm";
  let DiagnosticType = !strconcat("Invalid", Name);
}

// Condition assembly operand
def cdmCondAsmOp : AsmOperandClass {
  let Name = "Cond";
  let PredicateMethod = "isCond";
  let RenderMethod = "addCondOperands";
}

//===----------------------------------------------------------------------===//
// CDM instruction operands
//===----------------------------------------------------------------------===//

// 16-bit immediate operand (value or symbol).
def imm16Op : Operand<i16> {
  let EncoderMethod = "encodeImm<FK_Data_2, 2>";
  let ParserMatchClass = immSymAsmOp;
}

// 9-bit immediate operand (value only).
def imm9Op : Operand<i16> {
  let ParserMatchClass = imm9AsmOp;
}

// 6-bit immediate operand (value only).
def imm6Op : Operand<i16> {
  let ParserMatchClass = imm6AsmOp;
}

// 6-bit immediate operand (value only, negated).
def imm6SubOp : Operand<i16> {
  let ParserMatchClass = imm6SubAsmOp;
}

// 6-bit immediate operand for LDIImm6.
def imm6LDIOp : Operand<i16> {
  let EncoderMethod = "encodeImm<CDM::fixup_load_imm6, 0, true>";
  let ParserMatchClass = immSymAsmOp;
}

// 16-bit absoulute branch target operand (symbol only).
def imm16BrTarget : Operand<OtherVT> {
  let OperandType = "OPERAND_MEMORY";
  let EncoderMethod = "encodeImm<FK_Data_2, 2>";
  let ParserMatchClass = immSymAsmOp;
}

// 9-bit relative branch target operand (symbol only).
def imm9BrTarget : Operand<OtherVT> {
  let OperandType = "OPERAND_PCREL";
  let EncoderMethod = "encodeImm<CDM::fixup_branch_imm9, 0, true>";
  let ParserMatchClass = immSymAsmOp;
}

// 9-bit relative call target operand (symbol only).
def imm9CallTarget : Operand<i16> {
  let OperandType = "OPERAND_PCREL";
  let EncoderMethod = "encodeImm<CDM::fixup_call_imm9, 0, true>";
  let ParserMatchClass = immSymAsmOp;
}

// Condition code operand used in conditional branch instrcutions.
def cdmCond : Operand<OtherVT> {
  let PrintMethod = "printCondCode";
  let ParserMatchClass = cdmCondAsmOp;
}

// Offset operands for frame-relative loads/stores and addsp.
def imm9WordOffset : Operand<i16> {
  let EncoderMethod = "encodeOffset<2>";
  let ParserMatchClass = imm9EvenAsmOp;
}
def imm6WordOffset : Operand<i16> {
  let EncoderMethod = "encodeOffset<2>";
  let ParserMatchClass = imm6EvenAsmOp;
}
def imm6ByteOffset : Operand<i16> {
  let EncoderMethod = "encodeOffset<1>";
  let ParserMatchClass = imm6AsmOp;
}

// Small immediate value between 1 and 8 used in shift instructions.
// Encoded as val - 1.
def shamt : Operand<i16> {
  let EncoderMethod = "encodeShamt";
  let ParserMatchClass = shamtAsmOp;
  let OperandType = "OPERAND_IMMEDIATE";
}

// 9-bit interrupt vector operand.
def interruptVec : Operand<i16> {
  let ParserMatchClass = interruptVecAsmOp;
  let OperandType = "OPERAND_IMMEDIATE";
}

// reg+reg address operand used in load/store
// instructions with 3 operands.
def mem2Reg : Operand<iPTR> {
  let MIOperandInfo = (ops CPURegs, CPURegs);
  let OperandType = "OPERAND_MEMORY";
  let PrintMethod = "printMem2RegOperand";
}

//===----------------------------------------------------------------------===//
// CDM DAG patterns and transforms
//===----------------------------------------------------------------------===//

// Simple absolute address pattern.
def addr : ComplexPattern<iPTR, 1, "selectAddr", [], []>;

// reg+reg absolute address pattern.
def addr2Reg : ComplexPattern<iPTR, 2, "selectAddr2Reg", [], []>;

// Frame-relative address pattern.
def addrFrame : ComplexPattern<iPTR, 1, "selectAddrFrameIndex", [frameindex], []>;

// Immediate values with specific ranges (immM_N -> [M; N]).
def imm1_8 : ImmLeaf<i16, [{return (Imm >= 1) && (Imm <= 8);}]>;
def imm9_16 : ImmLeaf<i16, [{return (Imm >= 9) && (Imm <= 16);}]>;

// 6-bit immediate value pattern.
def imm6 : ImmLeaf<i16, [{return (Imm >= -64) && (Imm <= 63);}]>;

// Aligned load/store patterns.
def loadAligned : PatFrag<(ops node:$ptr), (load node:$ptr), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlign().value();
}]>;
def storeAligned  : PatFrag<(ops node:$val, node:$ptr), (store node:$val, node:$ptr), [{
  StoreSDNode *SD = cast<StoreSDNode>(N);
  return SD->getMemoryVT().getSizeInBits()/8 <= SD->getAlign().value();
}]>;

// Transforms LLVM cond codes to CDM cond codes
def cdmizeCondXform : SDNodeXForm<cond, [{
  return CurDAG->getTargetConstant(condCodeToCDMCond(N->get()), N, MVT::i16);
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
def ZERO: Inst0Op<0b0000, (outs), (ins), "zero", [(trap)]>;

// Halt processor
// Stops processor clock, changes processor status to HALTED.
def HALT: Inst0Op<0b0100, (outs), (ins), "halt", [(int_cdm_halt)]>;

// Wait for interrupt
// Stops processor clock until an interrupt or an exception
// occurs, changes processor status to WAITING.
def WAIT: Inst0Op<0b0101, (outs), (ins), "wait", [(int_cdm_wait)]>;

// Enable interrupts
// Sets I bit (bit 15) in PS thus enabling interrupts.
def EI: Inst0Op<0b0110, (outs), (ins), "ei", [(int_cdm_ei)]>;

// Disable interrupts
// Clears I bit (bit 15) in PS thus disabling interrupts.
def DI: Inst0Op<0b0111, (outs), (ins), "di", [(int_cdm_di)]>;

// Jump to subroutine (imm16 absolute address)
// Pushes PC to stack and then does unconditional
// absolute branch to address imm16.
def JSRImm16 : Inst0OpImm16<0b1000, (outs), (ins imm16BrTarget:$imm), "jsr\t$imm", []> {
  let isCall = 1;
  let isCodeGenOnly = 1;
}

let isTerminator = 1, isReturn = 1, isBarrier = 1 in {
  // Return from interrupt
  // Pops a return address and a processor status word from the stack and loads
  // them into PC and PS respectively.
  def RTI : Inst0Op<0b1001, (outs), (ins), "rti", [(cdm_rti)]>;

  // Pop PC
  // Pops a return address from the stack and loads it into PC.
  def POPC : Inst0Op<0b1011, (outs), (ins), "popc", [(cdm_rts)]>;
}

// Return from subroutine
// Alias for POPC.
def RTS : InstAlias<"rts", (POPC)>;

// Push PC
// Pushes PC to the stack.
def PUPC : Inst0Op<0b1010, (outs), (ins), "pupc", []> {
  let Uses = [SP];
  let Defs = [SP];
}

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
def LDIImm16 : Inst1OpImm16<0b0010, (outs CPURegs:$rd), (ins imm16Op:$imm), "ldi\t$rd, $imm", []> {
  let isReMaterializable = 1;
  let isCodeGenOnly = 1;
}

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
def LDPS : Inst1Op<0b0110, (outs CPURegs:$rd), (ins), "ldps\t$rd", [(set CPURegs:$rd, (int_cdm_ldps))]> {
  let Uses = [PSR];
}

// Store PS
// Copies value from rd to PS.
def STPS : Inst1Op<0b0111, (outs), (ins CPURegs:$rd), "stps\t$rd", [(int_cdm_stps CPURegs:$rd)]> {
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
def MOVE : Inst2Op<0b0000, (outs CPURegs:$rd), (ins CPURegs:$rs), "move\t$rs, $rd", []> {
  let Defs = [PSR]; // LLVM assumes that copyPhysReg (where MOVE is used) doesn't clobber anything.
                    // This "Defs" isn't considered.
}

//===----------------------------------------------------------------------===//
// mem2/mem3 format
//===----------------------------------------------------------------------===//

multiclass CDMLoad<bits<3> opcode, string mnem, SDPatternOperator OpNode> {
    // reg addressing mode
    def "" : InstMem2<{0, opcode}, (outs CPURegs:$rd), (ins CPURegs:$rs),
              !strconcat(mnem, "\t$rs, $rd"),
              [(set CPURegs:$rd, (OpNode addr:$rs))]>;

    // reg+reg addressing mode
    def "2Reg" : InstMem3<opcode, (outs CPURegs:$rd), (ins (mem2Reg $rs0, $rs1):$regs),
              !strconcat(mnem, "\t$regs, $rd"),
              [(set CPURegs:$rd, (OpNode addr2Reg:$regs))]>;
}

multiclass CDMStore<bits<3> opcode, string mnem, SDPatternOperator OpNode> {
    // reg addressing mode
    def "" : InstMem2<{0, opcode}, (outs), (ins CPURegs:$rd, CPURegs:$rs),
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
// Loads a byte from data memory pointed by rs to rd with zero extend.
defm LDB : CDMLoad<0b001, "ldb", zextloadi8>;

// Load signed byte
// Loads a byte from data memory pointed by rs to rd with sign extend.
defm LDSB : CDMLoad<0b010, "ldsb", sextloadi8>;

// Load constant word
// Loads 2 bytes from program memory pointed by rs to rd.
defm LCW : CDMLoad<0b011, "lcw", null_frag>;

// Load constant unsigned byte
// Loads a byte from program memory pointed by rs to rd with zero extend.
defm LCB : CDMLoad<0b100, "lcb", null_frag>;

// Load constant signed byte
// Loads a byte from program memory pointed by rs to rd with sign extend.
defm LCSB : CDMLoad<0b101, "lcsb", null_frag>;

// Store word
// Stores 2 bytes from rd to data memory pointed by rs.
defm STW : CDMStore<0b110, "stw", store>;

// Store byte
// Stores the least significant byte of rd to data memory pointed by rs.
defm STB : CDMStore<0b111, "stb", truncstorei8>;

// CDM8-like aliases
def LD : MnemonicAlias<"ld", "ldw">;
def LDC : MnemonicAlias<"ldc", "lcw">;
def ST : MnemonicAlias<"st", "stw">;

//===----------------------------------------------------------------------===//
// imm6 format
//===----------------------------------------------------------------------===//

class CDMLoadStack<bits<4> opcode, string mnem, Operand OffOp, SDPatternOperator OpNode>
    : InstImm6<opcode, (outs CPURegs:$rd), (ins OffOp:$imm),
               !strconcat(mnem, "\t$rd, $imm"),
               [(set CPURegs:$rd, (OpNode addrFrame:$imm))]>;

class CDMStoreStack<bits<4> opcode, string mnem, Operand OffOp, SDPatternOperator OpNode>
    : InstImm6<opcode, (outs), (ins CPURegs:$rd, OffOp:$imm),
               !strconcat(mnem, "\t$rd, $imm"),
               [(OpNode CPURegs:$rd, addrFrame:$imm)]>;

let Uses = [FP], PostEncoderMethod = "adjustImmOpCode<1, 9>" in {
  // Load word relative to fp
  // Loads a word from memory location fp + off to rd.
  def LSW : CDMLoadStack<0b0000, "lsw", imm6WordOffset, loadAligned>;

  // Load unsigned byte relative to fp
  // Loads a byte from memory location fp + off to rd with zero extend.
  def LSB : CDMLoadStack<0b0010, "lsb", imm6ByteOffset, zextloadi8>;

  // Load signed byte relative to fp
  // Loads a byte from memory location fp + off to rd with sign extend.
  def LSSB : CDMLoadStack<0b0100, "lssb", imm6ByteOffset, sextloadi8>;

  // Store word relative to fp
  // Stores 2 bytes from rd to memory location fp + off.
  def SSW : CDMStoreStack<0b0110, "ssw", imm6WordOffset, storeAligned>;

  // Store byte relative to fp
  // Stores the least significant byte of rd to memory location fp + off.
  def SSB : CDMStoreStack<0b1000, "ssb", imm6ByteOffset, truncstorei8>;
}

// Load small immediate to register
// Loads immediate value imm6 to rd.
def LDIImm6 : InstImm6<0b1010, (outs CPURegs:$rd), (ins imm6LDIOp:$imm), "ldi\t$rd, $imm",
                      [(set CPURegs:$rd, imm:$imm)]> {
  let isReMaterializable = 1;
}

let Defs = [PSR], isReMaterializable = 1, Constraints = "$rs = $rd" in {
  // Add immediate
  // Computes rd + imm6 and puts result in rd.
  def ADDImm6 : InstImm6<0b1100, (outs CPURegs:$rd), (ins CPURegs:$rs, imm6Op:$imm), "add\t$rs, $imm",
                         [(set CPURegs:$rd, (add CPURegs:$rs, imm6:$imm))]> {
    let PostEncoderMethod = "adjustImmOpCode<2, 9>";
  }

  // Subtract immediate
  // Computes rd - imm6 and puts result in rd.
  // Expands to add rd, -imm6
  def SUBImm6 : CDMPseudo<(outs CPURegs:$rd), (ins CPURegs:$rs, imm6SubOp:$imm), "sub\t$rs, $imm", []> {
    let isCodeGenOnly = 0;
    let isAsmParserOnly = 1;
  }
}

// Compare with immediate
// Performs indestructible subtraction, sets flags
// and thus compares numbers rd and imm6.
def CMPImm6 : InstImm6<0b1110, (outs), (ins CPURegs:$rd, imm6Op:$imm), "cmp\t$rd, $imm", []> {
  let Defs = [PSR];
  let PostEncoderMethod = "adjustImmOpCode<1, 9>";
}

// Increment register
// Increments register by 1 and sets all flags according to the result.
// Alias for ADD rd, 1
def INC : InstAlias<"inc\t$rd", (ADDImm6 CPURegs:$rd, 1)>;

// Decrement register
// Decrements register by 1 and sets all flags according to the result.
// Alias for ADD rd, -1
def DEC : InstAlias<"dec\t$rd", (ADDImm6 CPURegs:$rd, -1)>;

// Test register
// Compares register with 0 and sets Z, N flags according to the result.
// Alias for CMP rd, 0
def TST : InstAlias<"tst\t$rd", (CMPImm6 CPURegs:$rd, 0)>;

//===----------------------------------------------------------------------===//
// imm9 format
//===----------------------------------------------------------------------===//

// Software interrupt
// Triggers an interrupt with vector with number imm9.
def INT : InstImm9<0b0000, (outs), (ins interruptVec:$imm), "int\t$imm",
                   [(int_cdm_int timm:$imm)]>;

// Reset processor
// Fetches reset vector thus performing soft reset.
def RESET : InstImm9<0b0001, (outs), (ins interruptVec:$imm), "reset\t$imm",
                     [(int_cdm_reset timm:$imm)]>;

// Reset vector can be omitted
def : InstAlias<"reset", (RESET 0), 0>;

let Uses = [SP], Defs = [SP], PostEncoderMethod = "adjustImmOpCode<0, 9>" in {
  // Push immediate
  // Stores immediate value imm9 to the top of the stack.
  def PUSHImm9 : InstImm9<0b0010, (outs), (ins imm9Op:$imm), "push\t$imm", []> {
    let mayStore = 1;
  }

  // Add immediate to SP
  // Adds imm bytes to SP.
  def ADDSPImm9 : InstImm9<0b0100, (outs), (ins imm9WordOffset:$imm), "addsp\t$imm", []>;
}

// Jump to subroutine (imm9 offset)
// Pushes PC to stack and then does unconditional
// absolute branch to address PC + (imm * 2) + 2.
def JSRImm9 : InstImm9<0b0110, (outs), (ins imm9CallTarget:$imm), "jsr\t$imm", []> {
  let isCall = 1;
}

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
  let isCodeGenOnly = 1;
}

// Absolute conditional branch
// Loads imm16 to PC if the specified condition is true.
def BCondImm16 : InstBrAbs<(outs), (ins cdmCond:$cond, imm16BrTarget:$imm), "b$cond\t$imm", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
  let isCodeGenOnly = 1;
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
def BCondImm9 : InstBrRel<0b110, (outs), (ins cdmCond:$cond, imm9BrTarget:$imm),
                    "b$cond\t$imm", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

// No operation
// Performs no operation. Processor executes this instruction with no side
// effects except for updated PC.
def NOP : InstBrRel<0b111, (outs), (ins), "nop", []> {
  let cond = 0xF; // always false
  let imm = 0x1FF;
}

//===----------------------------------------------------------------------===//
// shifts format
//===----------------------------------------------------------------------===//

// Class for basic shift instructions.
multiclass Shift<bits<3> op, string instr_asm, SDPatternOperator OpNode> {
  let Defs = [PSR] in {
    // The shift amount is constant, and is between 1 and 8.
    def ""
        : InstShifts<op, (outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt),
                  !strconcat(instr_asm, "\t$rs, $rd, $shamt"),
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, imm1_8:$shamt))]>;

    // The shift amount is constant, and is greater than 8.
    // Needs to be expanded into two "regular" (amount between 1 and 8) instructions.
    def _LARGEAMT
        : CDMPseudo<(outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt), "",
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, imm9_16:$shamt))]> {
      let usesCustomInserter = true;
    }

    // The shift amount is stored in a register.
    // Expanded into an inline loop.
    def _LOOP
        : CDMPseudo<(outs CPURegs:$rd), (ins CPURegs:$rs, CPURegs:$shamt), "",
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, CPURegs:$shamt))]> {
      let usesCustomInserter = true;
    }
  }

  // CDM8-like 2 operand and 1 operand aliases
  def : InstAlias<!strconcat(instr_asm, "\t$rs, $rd"), (!cast<Instruction>(NAME) CPURegs:$rd, CPURegs:$rs, 1), 0>;
  def : InstAlias<!strconcat(instr_asm, "\t$rd, $shamt"), (!cast<Instruction>(NAME) CPURegs:$rd, CPURegs:$rd, shamt:$shamt), 0>;
  def : InstAlias<!strconcat(instr_asm, "\t$rd"), (!cast<Instruction>(NAME) CPURegs:$rd, CPURegs:$rd, 1), 0>;
}

// Class for 32-bit shift pseudos.
multiclass ShiftExt32<SDPatternOperator OpNode> {
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
defm ROR : Shift<0b100, "ror", rotr>;

// Cyclic left shift with carry
// Shifts left, bit 15 shifts to C, old C shifts to bit 0.
def RCL : InstShifts<0b101, (outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt), "rcl\t$rs, $rd, $shamt", []>;

// Cyclic right shift with carry
// Shifts right, bit 0 shifts to C, old C shifts to bit 15.
def RCR : InstShifts<0b110, (outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt), "rcr\t$rs, $rd, $shamt", []>;

// CDM8-like 2 operand and 1 operand aliases
def : InstAlias<"rcl\t$rs, $rd", (RCL CPURegs:$rd, CPURegs:$rs, 1), 0>;
def : InstAlias<"rcr\t$rs, $rd", (RCR CPURegs:$rd, CPURegs:$rs, 1), 0>;
def : InstAlias<"rcl\t$rd, $shamt", (RCL CPURegs:$rd, CPURegs:$rd, shamt:$shamt), 0>;
def : InstAlias<"rcr\t$rd, $shamt", (RCR CPURegs:$rd, CPURegs:$rd, shamt:$shamt), 0>;
def : InstAlias<"rcl\t$rd", (RCL CPURegs:$rd, CPURegs:$rd, 1), 0>;
def : InstAlias<"rcr\t$rd", (RCR CPURegs:$rd, CPURegs:$rd, 1), 0>;

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
  def SCL : InstAlu2<0b011, (outs CPURegs:$rd), (ins CPURegs:$rs), "scl\t$rs, $rd", [(set CPURegs:$rd, (and CPURegs:$rs, (i16 0xFF)))]>;
}

//CDM8-like 1 operand aliases
def : InstAlias<"neg\t$rd", (NEG CPURegs:$rd, CPURegs:$rd), 0>;
def : InstAlias<"not\t$rd", (NOT CPURegs:$rd, CPURegs:$rd), 0>;
def : InstAlias<"sxt\t$rd", (SXT CPURegs:$rd, CPURegs:$rd), 0>;
def : InstAlias<"scl\t$rd", (SCL CPURegs:$rd, CPURegs:$rd), 0>;

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
  def ADD : InstAlu3<0b100, (outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1), "add\t$rs0, $rs1, $rd",
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

// Clear register
// Sets register to 0 and sets flags according to its value before operation.
def CLR : InstAlias<"clr\t$rd", (SUB CPURegs:$rd, CPURegs:$rd, CPURegs:$rd), 0>;

// Bit set
// Sets bits in rs0 that are set in rs1. Alias for OR.
def BIS : InstAlias<"bis\t$rs0, $rs1, $rd", (OR CPURegs:$rd, CPURegs:$rs0, CPURegs:$rs1), 0>;

// CDM8-like 2 operand aliasas
def : InstAlias<"and\t$rs, $rd", (AND CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;
def : InstAlias<"or\t$rs, $rd", (OR CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;
def : InstAlias<"xor\t$rs, $rd", (XOR CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;
def : InstAlias<"add\t$rs, $rd", (ADD CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;
def : InstAlias<"addc\t$rs, $rd", (ADDC CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;
def : InstAlias<"bic\t$rs, $rd", (BIC CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;
def : InstAlias<"sub\t$rs, $rd", (SUB CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;
def : InstAlias<"bis\t$rs, $rd", (OR CPURegs:$rd, CPURegs:$rs, CPURegs:$rd), 0>;

//===----------------------------------------------------------------------===//
// alu3_ind format
//===----------------------------------------------------------------------===//

let Defs = [PSR] in {
  // Bit test
  // Performs indestructible AND and sets PS. If any of bits that
  // are set in bitmask rd are also set in rs, then Z is 0, otherwise Z is 1.
  def BIT : InstAlu3Ind<0b000, (outs), (ins CPURegs:$rs, CPURegs:$rd), "bit\t$rs, $rd", []>;

  // Compare
  // Performs indestructible subtraction, sets PS
  // and thus compares numbers in registers rs and rd.
  def CMPReg : InstAlu3Ind<0b110, (outs), (ins CPURegs:$rs, CPURegs:$rd), "cmp\t$rs, $rd", []>;
}

//===----------------------------------------------------------------------===//
// Pseudo instructions
//===----------------------------------------------------------------------===//

// Conditional branch
def PseudoBCondRR
    : CDMPseudo<(outs), (ins cdmCond:$cnd, CPURegs:$lhs, CPURegs:$rhs, imm16BrTarget:$target), "", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

def PseudoBCondRI
    : CDMPseudo<(outs), (ins cdmCond:$cnd, CPURegs:$lhs, imm6Op:$rhs, imm16BrTarget:$target), "", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

// Select
def PseudoSelectCC
    : CDMPseudo<(outs CPURegs:$dst),
                (ins CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, cdmCond:$cnd), "", []> {
  let usesCustomInserter = true;
}

def PseudoSelectCCI
    : CDMPseudo<(outs CPURegs:$dst),
                (ins CPURegs:$lhs, imm6Op:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, cdmCond:$cnd), "", []> {
  let usesCustomInserter = true;
}

// Stack adjustment required (by the ABI) before and after each call
let Defs = [SP], Uses = [SP] in {
  def PseudoCallAdjustStackDown
      : CDMPseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2), "",
                  [(callseq_start timm:$amt1, timm:$amt2)]>;
  def PseudoCallAdjustStackUp
      : CDMPseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2), "",
                  [(callseq_end timm:$amt1, timm:$amt2)]>;
}

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//

// Load symbols
def : Pat<(i16(load_sym tglobaladdr:$addr)), (LDIImm16 $addr)>;
def : Pat<(i16(load_sym texternalsym:$addr)), (LDIImm16 $addr)>;
def : Pat<(i16(load_sym tjumptable:$addr)), (LDIImm16 $addr)>;
def : Pat<(i16(load_sym tblockaddress:$addr)), (LDIImm16 $addr)>;
def : Pat<(i16(load_sym tconstpool:$addr)), (LDIImm16 $addr)>;

// Call (try to use short encoding first)
def : Pat<(cdm_call (i16 tglobaladdr:$target)), (JSRImm9 tglobaladdr:$target)>;
def : Pat<(cdm_call (i16 texternalsym:$target)), (JSRImm9 texternalsym:$target)>;

// Load frame address
def : Pat<(i16 addrFrame:$addr), (ADD (LDIImm6 imm:$addr), FP)>;

// Load byte from address with anyext
def : Pat<(i16(extloadi8 addr:$addr)), (LDB addr:$addr)>;
def : Pat<(i16(extloadi8 addr2Reg:$addr)), (LDB2Reg addr2Reg:$addr)>;
def : Pat<(i16(extloadi8 addrFrame:$src)), (LSB addrFrame:$src)>;

// Conditional branch
def : Pat<(brcc cond:$cnd, CPURegs:$lhs, imm6:$rhs, bb:$target),
          (PseudoBCondRI (cdmizeCondXform $cnd), CPURegs:$lhs, imm6:$rhs, imm16BrTarget:$target)>;
def : Pat<(brcc cond:$cnd, CPURegs:$lhs, CPURegs:$rhs, bb:$target),
          (PseudoBCondRR (cdmizeCondXform $cnd), CPURegs:$lhs, CPURegs:$rhs, imm16BrTarget:$target)>;

// Select
def : Pat<(selectcc CPURegs:$lhs, imm6:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, cond:$cnd),
          (PseudoSelectCCI CPURegs:$lhs, imm6:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, (cdmizeCondXform $cnd))>;
def : Pat<(selectcc CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, cond:$cnd),
          (PseudoSelectCC CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue, CPURegs:$vfalse, (cdmizeCondXform $cnd))>;
