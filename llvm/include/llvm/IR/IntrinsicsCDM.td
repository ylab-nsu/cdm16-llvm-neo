let TargetPrefix = "cdm" in {
  // Enable interrupts
  def int_cdm_ei : DefaultAttrsIntrinsic<[], [],
                       [IntrNoMem, IntrHasSideEffects, IntrWillReturn]>,
                   ClangBuiltin<"__builtin_cdm_ei">;

  // Disable interrupts
  def int_cdm_di : DefaultAttrsIntrinsic<[], [],
                       [IntrNoMem, IntrHasSideEffects, IntrWillReturn]>,
                   ClangBuiltin<"__builtin_cdm_di">;

  // Halt processor
  def int_cdm_halt : DefaultAttrsIntrinsic<[], [],
                         [IntrNoMem, IntrHasSideEffects, IntrNoReturn]>,
                     ClangBuiltin<"__builtin_cdm_halt">;

  // Wait for interrupt
  def int_cdm_wait : DefaultAttrsIntrinsic<[], [],
                         [IntrNoMem, IntrHasSideEffects]>,
                     ClangBuiltin<"__builtin_cdm_wait">;

  // Trigger software interrupt
  def int_cdm_int : DefaultAttrsIntrinsic<[], [llvm_i16_ty],
                        [IntrNoMem, IntrHasSideEffects, ImmArg<ArgIndex<0>>]>,
                    ClangBuiltin<"__builtin_cdm_int">;

  // Reset to interrupt vector
  def int_cdm_reset : DefaultAttrsIntrinsic<[], [llvm_i16_ty],
                        [IntrNoMem, IntrHasSideEffects, IntrNoReturn, ImmArg<ArgIndex<0>>]>,
                      ClangBuiltin<"__builtin_cdm_reset">;

  // Get PSR value
  def int_cdm_ldps : DefaultAttrsIntrinsic<[llvm_i16_ty], [],
                       [IntrNoMem, IntrHasSideEffects, IntrWillReturn]>,
                     ClangBuiltin<"__builtin_cdm_ldps">;

  // Set PSR value
  def int_cdm_stps : DefaultAttrsIntrinsic<[], [llvm_i16_ty],
                       [IntrNoMem, IntrHasSideEffects, IntrWillReturn]>,
                     ClangBuiltin<"__builtin_cdm_stps">;
}
