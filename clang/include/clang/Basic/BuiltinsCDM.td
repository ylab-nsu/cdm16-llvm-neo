include "clang/Basic/BuiltinsBase.td"

class CDMBuiltin<string prototype, string features = ""> : TargetBuiltin {
  let Spellings = ["__builtin_cdm_" # NAME];
  let Prototype = prototype;
  let Features = features;
}

let Attributes = [NoThrow] in {
  def ei : CDMBuiltin<"void()">;
  def di : CDMBuiltin<"void()">;
  def ldps : CDMBuiltin<"unsigned int()">;
  def stps : CDMBuiltin<"void(unsigned int)">;
  def wait : CDMBuiltin<"void()">;
  def "int" : CDMBuiltin<"void(unsigned int)">;
}

let Attributes = [NoThrow, NoReturn] in {
  def halt : CDMBuiltin<"void()">;
  def reset : CDMBuiltin<"void(unsigned int)">;
}
