// custom SDnode for return

def SDT_CDMRet : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def CDMRet : SDNode<"CDMISD::Ret",
                    SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDT_CDMCall : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def CDMCall : SDNode<"CDMISD::Call", SDT_CDMCall,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

// TODO: select addr not on frame (CDMDagToDagIsel)
def addr_on_frame
    : ComplexPattern<iPTR, 2,
                     "SelectAddrFrameIndex", [frameindex], []> {
   let WantsParent = true;
}

def addrRR : ComplexPattern<iPTR, 2, "SelectAddrRR", [], []>;
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []> {
   let WantsParent = true;
}

class CDMInst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  // Inst and Size: for tablegen(... -gen-emitter) and
  // tablegen(... -gen-disassembler) in CMakeLists.txt

  let Namespace = "CDM";

  let Size = 2;

  let OutOperandList = outs;
  let InOperandList = ins;

  let AsmString = asmstr;
  let Pattern = pattern;
  let hasDelaySlot = 0;
  // TODO: right now we don't need opcodes and stuff
}

class CDMPseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
    : CDMInst<outs, ins, asmstr, pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
}

class arithLogicR<string asm_instr, SDNode OpNode, bit isComm = 0>
    : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs0, CPURegs:$rs1),
              !strconcat(asm_instr, " $rs0, $rs1, $rd"),
              [(set CPURegs:$rd, (OpNode CPURegs:$rs0, CPURegs:$rs1))]> {
  let isCommutable = isComm;
  let isReMaterializable = 1;
  let Defs = [PSR];
}

def ADD : arithLogicR<"add", add, 1>;
def SUB : arithLogicR<"sub", sub, 0>;

def AND : arithLogicR<"and", and, 1>;
def OR : arithLogicR<"or", or, 1>;
def XOR : arithLogicR<"xor", xor, 1>;

def immZext6 : ImmLeaf<i16, [{return (Imm >= -64) && (Imm <= 63);}]>;

// thanks avr
def imm16_neg_XFORM : SDNodeXForm<imm, [{
                    return CurDAG->getTargetConstant(-N->getAPIntValue(),
                                                     SDLoc(N), MVT::i16);
                  }]>;
// thanks avr
def imm6_neg : PatLeaf<(imm), [{
                            int64_t val = -N->getSExtValue();
                            return val >= 0 && val < 64;
                          }],
                       imm16_neg_XFORM>;

class arithLogicRi<string asm_instr, SDNode OpNode>
    : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs, i16imm:$imm),
              !strconcat(asm_instr, "\t$rs, $imm"),
              [(set CPURegs:$rd, (OpNode CPURegs:$rs, immZext6:$imm))]> {
  let isReMaterializable = 1;
  let Defs = [PSR];
  let Constraints = "$rs = $rd";
}
// end thanks avr

def NEG : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs),
              "neg\t$rs, $rd", []>{
  let Defs = [PSR];
}

def : Pat<(i16(sub 0, i16:$val)), (NEG $val)>;

def ADDI : arithLogicRi<"add", add>;
def SUBI : arithLogicRi<"sub", sub>;

def : Pat<(add i16:$src1, imm6_neg:$src2), (SUBI i16:$src1,
                                               (imm16_neg_XFORM imm:$src2))>;

// Shift amount
def shamt : Operand<i16>;

// Immediate values with specific ranges (immM_N -> [M; N])
def imm1_8 : ImmLeaf<i16, [{return (Imm >= 1) && (Imm <= 8);}]>;
def imm9_16 : ImmLeaf<i16, [{return (Imm >= 9) && (Imm <= 16);}]>;

// Class for basic shift instructions.
multiclass Shift<string instr_asm, SDNode OpNode> {
  let Defs = [PSR] in {
    // The shift amount is constant, and is between 1 and 8.
    def ""
        : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt),
                  !strconcat(instr_asm, "\t$rs, $rd, $shamt"),
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, imm1_8:$shamt))]>;

    // The shift amount is constant, and is greater than 8.
    // Needs to be expanded into two "regular" (amount between 1 and 8) instructions.
    def _LARGEAMT
        : CDMPseudo<(outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt),
                  !strconcat(!strconcat(!strconcat("!", instr_asm), "_LARGEAMT"), "\t$rs, $rd, $shamt"),
                  [(set CPURegs:$rd, (OpNode CPURegs:$rs, imm9_16:$shamt))]> {
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

// Basic shifts
defm SHL : Shift<"shl", shl>;
defm SHRA : Shift<"shra", sra>;
defm SHR : Shift<"shr", srl>;
defm ROL : Shift<"rol", rotl>;
defm ROR : Shift<"ror", rotr>;

// Class for rotate through carry instrctions
class RotateCarry<string instr_asm> 
    : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs, shamt:$shamt),
          !strconcat(instr_asm, "\t$rs, $rd, $shamt"), []>;

// Rotate through carry instructions
def RCL : RotateCarry<"rcl">;
def RCR : RotateCarry<"rcr">;

// Extended shift node types.
def SDTIntShiftQuadOp : SDTypeProfile<4, 5, [
  SDTCisInt<0>, SDTCisSameAs<1, 0>,
  SDTCisSameAs<2, 0>, SDTCisSameAs<3, 0>,
  SDTCisSameAs<4, 0>, SDTCisSameAs<5, 0>,
  SDTCisSameAs<6, 0>, SDTCisSameAs<7, 0>, SDTCisInt<8>
]>;

def shl_ext32 : SDNode<"CDMISD::SHL_EXT32", SDTIntShiftPairOp>;
def srl_ext32 : SDNode<"CDMISD::SRL_EXT32", SDTIntShiftPairOp>;
def sra_ext32 : SDNode<"CDMISD::SRA_EXT32", SDTIntShiftPairOp>;
def shl_ext64 : SDNode<"CDMISD::SHL_EXT64", SDTIntShiftQuadOp>;
def srl_ext64 : SDNode<"CDMISD::SRL_EXT64", SDTIntShiftQuadOp>;
def sra_ext64 : SDNode<"CDMISD::SRA_EXT64", SDTIntShiftQuadOp>;

// Classes for extended shift operations.
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
// so that it doesn't try to swap the values at the same time as shifting them.

// Extended shift pseudos
defm SHL_EXT32 : ShiftExt32<shl_ext32>;
defm SHR_EXT32 : ShiftExt32<srl_ext32>;
defm SHRA_EXT32 : ShiftExt32<sra_ext32>;
defm SHL_EXT64 : ShiftExt64<shl_ext64>;
defm SHR_EXT64 : ShiftExt64<srl_ext64>;
defm SHRA_EXT64 : ShiftExt64<sra_ext64>;

// load
class AlignedLoad<PatFrag Node> : PatFrag<(ops node:$ptr), (Node node:$ptr), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlign().value();
}]>;
class AlignedStore<PatFrag Node>
    : PatFrag<(ops node:$val, node:$ptr), (Node node:$val, node:$ptr), [{
  StoreSDNode *SD = cast<StoreSDNode>(N);
  return SD->getMemoryVT().getSizeInBits()/8 <= SD->getAlign().value();
}]>;

def load_a : AlignedLoad<load>;
def store_a : AlignedStore<store>;

class LoadMStack<string instr_asm, PatFrag OpNode, Operand MemOperand,
                 ComplexPattern AddrPattern, bit Pseudo>
    : CDMInst<(outs CPURegs:$ra), (ins MemOperand:$addr),
              !strconcat(instr_asm, "\t$ra, $addr"),
              [(set CPURegs:$ra, (OpNode AddrPattern:$addr))]> {
  let isPseudo = Pseudo;
}

class StoreMStack<string instr_asm, PatFrag OpNode, Operand MemOperand,
                  ComplexPattern AddrPattern, bit Pseudo>
    : CDMInst<(outs), (ins CPURegs:$ra, MemOperand:$addr),
              !strconcat(instr_asm, "\t$ra, $addr"),
              [(OpNode CPURegs:$ra, AddrPattern:$addr)]> {
  let isPseudo = Pseudo;
}

def simm16 : Operand<i16> {}

// def uimm16: Operand<ui16>{

// }

// Selected in SelectAddr
def mem : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  // Why not more(GRPOUT)
  let MIOperandInfo = (ops CPURegs,
      simm16); // second operand SHOULD be always zero
  let EncoderMethod = "getMemEncoding";
}

def memRR : Operand<iPTR> {
  let PrintMethod = "printMemRROperand";
  let MIOperandInfo = (ops CPURegs, CPURegs);
}

let Uses = [FP] in {
  // stack words
  def lsw : LoadMStack<"lsw", load_a, mem, addr_on_frame, 0>;
  def ssw : StoreMStack<"ssw", store_a, mem, addr_on_frame, 0>;
  // stack bytes
  def ssb : StoreMStack<"ssb", truncstorei8, mem, addr_on_frame, 0>;
  def lsb : LoadMStack<"lsb", zextloadi8, mem, addr_on_frame, 0>;
  def lssb : LoadMStack<"lssb", sextloadi8, mem, addr_on_frame, 0>;
}
def : Pat<(i16(extloadi8 addr_on_frame:$src)), (lsb addr_on_frame:$src)>;

// TODO: use TABLES?
class LoadM<string instr_asm, PatFrag OpNode, Operand MemOperand,
            ComplexPattern AddrPattern, bit Pseudo>
    : CDMInst<(outs CPURegs:$ra), (ins MemOperand:$addr),
              !strconcat(instr_asm, "\t$addr, $ra"),
              [(set CPURegs:$ra, (OpNode AddrPattern:$addr))]> {
  let isPseudo = Pseudo;
}

class StoreM<string instr_asm, PatFrag OpNode, Operand MemOperand,
             ComplexPattern AddrPattern, bit Pseudo>
    : CDMInst<(outs), (ins CPURegs:$ra, MemOperand:$addr),
              !strconcat(instr_asm, "\t$addr, $ra"),
              [(OpNode CPURegs:$ra, AddrPattern:$addr)]> {
  let isPseudo = Pseudo;
}

// WORD store
def stwRR : StoreM<"stw", store, memRR, addrRR, 0>;
def stw : StoreM<"stw", store, mem, addr, 0>;
// WORD load
def ldwRR : LoadM<"ldw", load, memRR, addrRR, 0>;
def ldw : LoadM<"ldw", load, mem, addr, 0>;

// BYTE load zero extend
def ldbRR : LoadM<"ldb", zextloadi8, memRR, addrRR, 0>;
def ldb : LoadM<"ldb", zextloadi8, mem, addr, 0>;
// BYTE load sign extend
def ldsbRR : LoadM<"ldsb", sextloadi8, memRR, addrRR, 0>;
def ldsb : LoadM<"ldsb", sextloadi8, mem, addr, 0>;
// BYTE store truncate
def stbRR : StoreM<"stb", truncstorei8, memRR, addrRR, 0>;
def stb : StoreM<"stb", truncstorei8, mem, addr, 0>;

def : Pat<(i16(extloadi8 addrRR:$src)), (ldbRR addrRR:$src)>;
def : Pat<(i16(extloadi8 addr:$src)), (ldb addr:$src)>;

// TODO: ArithLogic?
// TODO: Why not just sext?
def Sext : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs), "sxt\t$rs, $rd", []> {
  let Defs = [PSR];
}

def : Pat<(i16(sext_inreg i16:$val, i8)), (Sext $val)>;

// sextloadi8
// zextloadi8
// truncstorei8

// def stw: CDMInst<(outs), (ins CPURegs:$ra, mem:$addr), "stw\t$addr, $ra",
// [(store CPURegs:$ra, addr:$addr)]>; def ldw: CDMInst<(outs CPURegs:$ra), (ins
// mem:$addr), "ldw\t$addr, $ra", [(set CPURegs:$ra,(load addr:$addr))]>; ldi

def imm16 : PatLeaf<(imm), [{return isInt<16>(N->getValue());}]>;
class LoadImm<Operand Imm>
    : CDMInst<(outs CPURegs:$ra), (ins Imm:$imm), "ldi\t$ra, $imm", []> {
  // TODO; what is this?
  let isReMaterializable = 1;
  // COSTYL
  // let Defs = [PSR];
}

def ldi : LoadImm<simm16>;

let isReturn = 1, isTerminator = 1, isBarrier = 1,
    hasCtrlDep = 1 in def PseudoRet : CDMPseudo<(outs), (ins), "", [(CDMRet)]>;

/*
 TODO: RET, RetLR
 LD: LoadM32 -> LoadM -> FMem -> FL -> CpuoInst -> Instruction
 My LD: LoadM32 -> LoadM -> CDMInst
*/
def load_sym : SDNode<"CDMISD::LOAD_SYM", SDTIntUnaryOp>;
def : Pattern<(i16(load_sym tglobaladdr:$addr)), [(ldi $addr)]>;
def : Pattern<(i16(load_sym tjumptable:$addr)), [(ldi $addr)]>;

def my_cool_pat_for_ldi : Pat<(i16 imm:$imm), (ldi imm:$imm)>;

class RetBase<string instr_asm> : CDMInst<(outs), (ins), instr_asm, []> {
  // TODO: check this flags
  let isReturn = 1;
  let isCodeGenOnly = 1;
  let hasCtrlDep = 1;
  let hasExtraSrcRegAllocReq = 1;
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
}

def rts : RetBase<"rts">;
def rti : RetBase<"rti">;

def brtarget : Operand<OtherVT> { let OperandType = "OPERAND_MEMORY"; }

def BR : CDMInst<(outs), (ins brtarget:$addr), "br\t$addr", [(br bb:$addr)]> {
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
}

// branch indirect
def stpc
    : CDMInst<(outs), (ins CPURegs:$rs), "stpc\t$rs", [(brind CPURegs:$rs)]> {
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
  let isIndirectBranch = 1;
}

// def
// class CondBR<string instr_asm>: CDMInst<(outs), (ins SR:$psr,
// brtarget:$addr),
//                                         !strconcat(instr_asm, "\t$addr"),
//                                         []>{
//                                           let isBranch = 1;
//                                           let isTerminator = 1;
//                                         }

def cond_val : Operand<OtherVT> { let PrintMethod = "printCondCode"; }

def cdmize_cond : SDNodeXForm<cond, [{
  return CurDAG->getTargetConstant(CCToCondOp(N->get()), N, MVT::i16);
}]>;

def BCond : CDMInst<(outs), (ins cond_val:$cond, brtarget:$addr),
                    "b$cond\t$addr", []> {
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

def PseudoSelectCC
    : CDMPseudo<(outs CPURegs:$dst),
              (ins CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue, CPURegs:$vfalse,
                  cond_val:$cnd),
              "",
              []> {
  let usesCustomInserter = true;
}

def PseudoSelectCCI
    : CDMPseudo<(outs CPURegs:$dst),
              (ins CPURegs:$lhs, i16imm:$rhs, CPURegs:$vtrue, CPURegs:$vfalse,
                  cond_val:$cnd),
              "",
              []> {
  let usesCustomInserter = true;
}

def : Pat<(selectcc CPURegs:$lhs, immZext6:$rhs, CPURegs:$vtrue, CPURegs:$vfalse,
              cond:$cnd),
          (PseudoSelectCCI CPURegs:$lhs, immZext6:$rhs, CPURegs:$vtrue,
              CPURegs:$vfalse, (cdmize_cond $cnd))>;

def : Pat<(selectcc CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue, CPURegs:$vfalse,
              cond:$cnd),
          (PseudoSelectCC CPURegs:$lhs, CPURegs:$rhs, CPURegs:$vtrue,
              CPURegs:$vfalse, (cdmize_cond $cnd))>;

def PseudoBCondRR : CDMPseudo<(outs), (ins cond_val:$cnd, CPURegs:$lhs, CPURegs:$rhs, brtarget:$addr), "", []>{
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

def PseudoBCondRI : CDMPseudo<(outs), (ins cond_val:$cnd, CPURegs:$lhs, i16imm:$rhs, brtarget:$addr), "", []>{
  let isTerminator = 1;
  let isBranch = 1;
  let Uses = [PSR];
}

def : Pat<(brcc cond:$cnd, CPURegs:$lhs, immZext6:$rhs, bb:$target),
          (PseudoBCondRI (cdmize_cond $cnd), CPURegs:$lhs, immZext6:$rhs, brtarget:$target)>;

def : Pat<(brcc cond:$cnd, CPURegs:$lhs, CPURegs:$rhs, bb:$target),
          (PseudoBCondRR (cdmize_cond $cnd), CPURegs:$lhs, CPURegs:$rhs, brtarget:$target)>;

def CMP : CDMInst<(outs), (ins CPURegs:$ra, CPURegs:$rb), "cmp\t$ra, $rb", []> {
  let Defs = [PSR];
}

def CMPI : CDMInst<(outs), (ins CPURegs:$r, i16imm:$imm), "cmp\t$r, $imm", []> {
  let Defs = [PSR];
}

def TST : CDMInst<(outs), (ins CPURegs:$rs), "tst\t$rs", []> {
  let Defs = [PSR];
}

// TODO: maybe use PSR from ALU  instructions in branches?
// def : Pat<(brcond CPURegs:$cond, brtarget:$addr), ()>mo

// def : Pat<(brcond (i16 (setlt CPURegs:$ra, CPURegs:$rb)), bb:$dst),
//           (BLT (CMP CPURegs:$ra, CPURegs:$rb), bb:$dst)>;
def calltarget : Operand<iPTR> {
  let OperandType = "OPERAND_PCREL";
  // let PrintMethod = "printCallTarget";
}

def JSR : CDMInst<(outs), (ins calltarget:$target, variable_ops),
                  "jsr\t$target", []> {
  let isCall = 1;
}

def JSRR : CDMInst<(outs), (ins CPURegs:$rs, variable_ops), "jsrr\t$rs", []> {
  let isCall = 1;
}

def : Pat<(CDMCall(i16 tglobaladdr:$target)), (JSR tglobaladdr:$target)>;
def : Pat<(CDMCall(i16 texternalsym:$target)), (JSR texternalsym:$target)>;

def uimm16 : Operand<i16>;
def SDT_CDMCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;
def SDT_CDMCallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
// These are target-independent nodes, but have target-specific formats.
def callseq_start : SDNode<"ISD::CALLSEQ_START",
                           SDT_CDMCallSeqStart, [SDNPHasChain, SDNPOutGlue]>;
def callseq_end
    : SDNode<"ISD::CALLSEQ_END",
             SDT_CDMCallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

let Defs = [SP], Uses = [SP] in {
  def ADJCALLSTACKDOWN : CDMPseudo<(outs), (ins uimm16:$amt1, uimm16:$amt2),
                                   "!ADJCALLSTACKDOWN $amt1",
                                   [(callseq_start timm:$amt1, timm:$amt2)]>;
  def ADJCALLSTACKUP
      : CDMPseudo<
            (outs), (ins uimm16:$amt1, uimm16:$amt2),
            "!ADJCALLSTACKUP $amt1", [(callseq_end timm:$amt1, timm:$amt2)]>;
}

// TODO: convert to ArithLogic ???
def MOVE
    : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs), "move\t$rs, $rd", []> {
  // let Defs = [PSR];
}

// push, pop, ldsp, addsp
// TODO: this is not selected automatically, probably there should be something
// more
let Uses = [SP], Defs = [SP] in {
  def POP : CDMInst<(outs CPURegs:$rd), (ins), "pop\t$rd", []> {
    let mayLoad = 1;
  }

  def PUSH : CDMInst<(outs), (ins CPURegs:$rs), "push\t$rs", []> {
    let mayStore = 1;
  }
}

def LDSP : CDMInst<(outs CPURegs:$rd), (ins), "ldsp\t$rd", []> {
  let Uses = [SP];
}

def STSP : CDMInst<(outs), (ins CPURegs:$rd), "stsp\t$rd", []> {
  let Defs = [SP];
}

def ADDSP : CDMInst<(outs), (ins simm16:$imm), "addsp\t$imm", []> {
  let Uses = [SP];
  let Defs = [SP];
}

def load_frame_adderss
    : CDMInst<(outs CPURegs:$rd), (ins CPURegs:$rs), "add\t$rs, fp, $rd", []> {
  let isCodeGenOnly = true;
  let Uses = [FP];
}

def : Pat<(i16 addr_on_frame:$addr), (load_frame_adderss(ldi CPURegs:$addr))>;
