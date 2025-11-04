//===-- CDMRegisterInfo.td - CDM Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the CDM register file
//===----------------------------------------------------------------------===//

// We have banks of 16 registers each.
class CDMReg<bits<16> Enc, string n> : Register<n> {
  // For tablegen(... -gen-emitter)  in CMakeLists.txt
  let HWEncoding = Enc;

  let Namespace = "CDM";
}

// CDM CPU Registers
class CDMGPRReg<bits<16> Enc, string n> : CDMReg<Enc, n>;

//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
let Namespace = "CDM" in {
  //@ General Purpose Registers
  def R0 : CDMGPRReg<0, "r0">, DwarfRegNum<[0]>;
  def R1 : CDMGPRReg<1, "r1">, DwarfRegNum<[1]>;
  def R2 : CDMGPRReg<2, "r2">, DwarfRegNum<[2]>;
  def R3 : CDMGPRReg<3, "r3">, DwarfRegNum<[3]>;
  def R4 : CDMGPRReg<4, "r4">, DwarfRegNum<[4]>;
  def R5 : CDMGPRReg<5, "r5">, DwarfRegNum<[5]>;
  def R6 : CDMGPRReg<6, "r6">, DwarfRegNum<[6]>;
  def FP : CDMGPRReg<7, "fp">, DwarfRegNum<[7]>;

  def PC : CDMReg<-1, "pc">, DwarfRegNum<[20]>;
  def PSR : CDMReg<-1, "psr">, DwarfRegNum<[21]>;
  def SP : CDMReg<-1, "sp">, DwarfRegNum<[22]>;
}

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def CPURegs
    : RegisterClass<"CDM", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, FP)>;

//@Status Registers class
def SPECIAL : RegisterClass<"CDM", [i16], 16, (add PC, PSR, SP)>;

def SR : RegisterClass<"CDM", [i16], 16, (add PSR)>;

//@Co-processor 0 Registers class
// def C0Regs : RegisterClass<"CDM", [i32], 32, (add PC, EPC)>;

// test
// dummy1
