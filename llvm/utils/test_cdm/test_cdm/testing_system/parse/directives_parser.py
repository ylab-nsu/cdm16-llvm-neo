from test_cdm.testing_system.parse.directive import Directive
from test_cdm.testing_system.processor import ProcessorInfo
from test_cdm.testing_system.assertions import Assertion, RegisterAssertion, MemoryAssertion, AbsoluteSectionAssertion
from .test_parsing_error import TestParsingError

def parse_int(string: str, numsize: int) -> int:
  try:
    num = int(string, 0)
  except ValueError:
    raise TestParsingError(f'Cannot parse "{string}" to int')

  if not (-2**(numsize - 1) <= num < 2**numsize):
    raise TestParsingError(f'Int value out of bound: {num}')

  if num < 0:
    return num + 2**numsize
  return num

def parse_address(string: str, memsize: int) -> int:
  try:
    num = int(string, 0)
  except ValueError:
    raise TestParsingError(f'Cannot parse "{string} to address"')

  if not (0 <= num < memsize):
    raise TestParsingError(f'Address value out of bound: {num}')

  return num

def parse_register_directive(directive: Directive, processor_info: ProcessorInfo) -> RegisterAssertion:
  if not len(directive.head) == 1:
    raise TestParsingError(f"Wrong amount of arguments in head, expected 1, got: {len(directive.head)}")
  if not len(directive.body) == 1:
    raise TestParsingError(f"Wrong amount of arguments in body, expected 1, got: {len(directive.body)}")

  reg = directive.head[0]

  if not reg in processor_info.regs:
    raise TestParsingError(f'Incorrect register name: "{reg}", expected one of: {processor_info.regs}')

  reg_num = processor_info.regs.index(reg)

  val = parse_int(directive.body[0], processor_info.regsizes[reg_num])

  return RegisterAssertion(reg, val)

def parse_memory_directive(directive: Directive, processor_info: ProcessorInfo) -> MemoryAssertion:
  if not len(directive.head) == 1:
    raise TestParsingError(f"Wrong amount of arguments in head, expected 1, got: {len(directive.head)}")
  if not directive.body:
    raise TestParsingError("Empty content")

  address = parse_address(directive.head[0], processor_info.memsize)
  parsed_content = []

  try:
    for byte in directive.body:
      byte_as_int = int(byte, 16)
      if not (0 <= byte_as_int < 256):
        raise TestParsingError(f'Byte value out of range: "{byte}"')
      parsed_content.append(byte_as_int)
  except ValueError:
    raise TestParsingError(f'Cannot parse byte from : "{byte}"')

  return MemoryAssertion(address, parsed_content)

def parse_symbol_directive(directive: Directive, processor_info: ProcessorInfo) -> AbsoluteSectionAssertion:
  if not len(directive.head) == 1:
    raise TestParsingError(f"Wrong amount of arguments in head, expected 1, got: {len(directive.head)}")
  if not directive.body:
    raise TestParsingError("Empty content")

  parsed_content = []

  try:
    for byte in directive.body:
      byte_as_int = int(byte, 16)
      if not (0 <= byte_as_int < 256):
        raise TestParsingError(f'Byte value out of range: "{byte}"')
      parsed_content.append(byte_as_int)
  except ValueError:
    raise TestParsingError(f'Cannot parse byte from : "{byte}"')

  return AbsoluteSectionAssertion(directive.head[0], parsed_content)

def parse_directive(directive: Directive, processor_info: ProcessorInfo) -> Assertion:
  match directive.name:
    case "reg":
      return parse_register_directive(directive, processor_info)
    case "mem":
      return parse_memory_directive(directive, processor_info)
    case "sym":
      return parse_symbol_directive(directive, processor_info)
    case _:
      raise TestParsingError(f'Incorrect directive name: "{directive.name}"')
