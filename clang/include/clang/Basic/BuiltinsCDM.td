include "clang/Basic/BuiltinsBase.td"

class CDMBuiltin<string prototype, string features = ""> : TargetBuiltin {
  let Spellings = ["__builtin_cdm_" # NAME];
  let Prototype = prototype;
  let Features = features;
}

let Attributes = [NoThrow] in {
  def int_enable : CDMBuiltin<"void()">;
  def int_disable : CDMBuiltin<"void()">;
  def get_ps : CDMBuiltin<"unsigned int()">;
  def set_ps : CDMBuiltin<"void(unsigned int)">;
  def wait : CDMBuiltin<"void()">;
  def interrupt : CDMBuiltin<"void(unsigned int)">;
}

let Attributes = [NoThrow, NoReturn] in {
  def halt : CDMBuiltin<"void()">;
  def reset : CDMBuiltin<"void(unsigned int)">;
}
