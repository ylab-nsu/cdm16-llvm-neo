def CC_CDM : CallingConv<[CCIfType<[i8], CCPromoteToType<i16>>,
                          CCIfType<[i16], CCAssignToReg<[R0, R1, R2, R3]>>,
                          CCAssignToStack<2, 2>,
]>;

def RetCC_CDM : CallingConv<[
                    // i16 are returned in register(s)
                    CCIfType<[i16], CCAssignToReg<[R0, R1, R2, R3]>>]>;

def CSR_O16 : CalleeSavedRegs<(add (sequence "R%u", 4, 6), FP)>;
def CSR_O16_ALL : CalleeSavedRegs<(add (sequence "R%u", 0, 6), FP)>; // Order is important here, since ISRs have a single parameter - a pointer
                                                                     // to the previous context on the stack, which needs to have specific layout
