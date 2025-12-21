from pathlib import Path
from .util.directives_iterating import iter_directives
from .structs import *
from .exceptions import TestParsingError

def parse_int(string, numsize):
  try:
    num = int(string, 0)
  except ValueError:
    raise TestParsingError(f'Cannot parse "{string}" to int')

  if not (-2**(numsize - 1) <= num < 2**numsize):
    raise TestParsingError(f'Int value out of bound: {num}')

  if num < 0:
    return num + 2**numsize
  return num

def parse_address(string, memsize):
  try:
    num = int(string, 0)
  except ValueError:
    raise TestParsingError(f'Cannot parse "{string} to address"')

  if not (0 <= num < memsize):
    raise TestParsingError(f'Address value out of bound: {num}')

  return num

def parse_register_directive(head, body, assertions, processor_info):
  if not len(head) == 1:
    raise TestParsingError(f"Wrong amount of arguments in head, expected 1, got: {len(head)}")
  if not len(body) == 1:
    raise TestParsingError(f"Wrong amount of arguments in body, expected 1, got: {len(body)}")

  reg = head[0]

  if not reg in processor_info.regs:
    raise TestParsingError(f'Incorrect register name: "{reg}", expected one of: {processor_info.regs}')

  reg_num = processor_info.regs.index(reg)

  val = parse_int(body[0], processor_info.regsizes[reg_num])

  assertions.append(RegisterAssertion(reg, val))

def parse_memory_directive(head, content, assertions, processor_info):
  if not len(head) == 1:
    raise TestParsingError(f"Wrong amount of arguments in head, expected 1, got: {len(head)}")
  if not content:
    raise TestParsingError("Empty content")

  address = parse_address(head[0], processor_info.memsize)
  parsed_content = []

  try:
    for byte in content:
      byte_as_int = int(byte, 16)
      if not (0 <= byte_as_int < 256):
        raise TestParsingError(f'Byte value out of range: "{byte}"')
      parsed_content.append(byte_as_int)
  except ValueError:
    raise TestParsingError(f'Cannot parse byte from : "{byte}"')

  assertions.append(MemoryAssertion(address, parsed_content))

def parse_symbol_directive(head, content, assertions, processor_info):
  if not len(head) == 1:
    raise TestParsingError(f"Wrong amount of arguments in head, expected 1, got: {len(head)}")
  if not content:
    raise TestParsingError("Empty content")

  parsed_content = []

  try:
    for byte in content:
      byte_as_int = int(byte, 16)
      if not (0 <= byte_as_int < 256):
        raise TestParsingError(f'Byte value out of range: "{byte}"')
      parsed_content.append(byte_as_int)
  except ValueError:
    raise TestParsingError(f'Cannot parse byte from : "{byte}"')

  assertions.append(AbsoluteSectionAssertion(head[0], parsed_content))

def parse_directive(directive, assertions, processor_info):
  
  # Get directive name, objects it checks(e.g. name of register or address) and body
  name, head, body = directive
  head = [] if not head else list(map(str.strip, head.split(',')))
  body = body.strip()
  body = [] if not body else body.split()

  match name:
    case "reg":
      parse_register_directive(head, body, assertions, processor_info)
    case "mem":
      parse_memory_directive(head, body, assertions, processor_info)
    case "sym":
      parse_symbol_directive(head, body, assertions, processor_info)
    case _:
      raise TestParsingError(f'Incorrect directive name: "{command}"')

def parse_all_directives(filepath, assertions, processor_info):
  if not filepath.is_file():
    raise ValueError("Expected regular file") 
  
  for line, *directive in iter_directives(filepath):
    try:
      parse_directive(directive, assertions, processor_info)
    except TestParsingError as e:
      raise TestParsingError(f'{filepath.name}:{line} {e.message}')

def parse_test(filepath, processor_info):
  files = []
  assertions = []
  
  # Initialize files
  if filepath.is_file():
    files.append(filepath.absolute())
  elif filepath.is_dir():
    for file in filter(Path.is_file, filepath.iterdir()):
      files.append(file)
  else:
    raise ValueError("Expected path to regular file or directory")
  
  # Add commons
  commons = Path(__file__).parent.parent.parent.absolute() / "commons"
  for file in filter(Path.is_file, commons.iterdir()):
    if not file.stem in map(lambda f : f.stem, files):
      files.append(file)
  
  # Parse all directives
  for file in filter(lambda f : f.suffix == '.c', files):
    parse_all_directives(file, assertions, processor_info)
  
  return BaseTest(filepath.name, files, assertions)

def get_tests(test_dir, processor_info):
  tests = []
  for test in filter((lambda f : f.is_file() or f.is_dir()), test_dir.iterdir()):
    tests.append(parse_test(test, processor_info))

  return tests
