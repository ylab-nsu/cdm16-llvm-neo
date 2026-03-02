import os
import tempfile
from typing import cast
from io import TextIOBase
from dataclasses import dataclass
from pathlib import Path
from collections.abc import Generator
from testing_system.test_case import TestCase
from testing_system.parse.directive import Directive
from testing_system.toolchain import *
from testing_system.cocoemu import CocoemuConnection
from testing_system.util.errors_printing import print_error_big
from .assertions import Assertion, AbsoluteSectionAssertion

_START_OF_ABSOLUTE_SECTIONS = 0x80

def place_all_absolute_sections(absolute_sections: list[AbsoluteSectionAssertion]) -> Path:
  next_address = _START_OF_ABSOLUTE_SECTIONS

  with tempfile.NamedTemporaryFile(suffix = '.asm', delete=False, mode='wt') as temp:
    for sec in absolute_sections:
      sec.address = next_address
      temp.write(f"asect {next_address}\n{sec.symbol}> ds {len(sec.content)}\n")
      next_address += len(sec.content)
    temp.write('end.\n')
    return Path(temp.name)

@dataclass
class CocasEndToEndTestCase(TestCase):
  assertions: list[Assertion]
  clang_path: Path
  include_paths: list[Path]

  def run_tests(self, connection: CocoemuConnection, errors_stream: TextIOBase) -> Generator[bool]:
    base_cocas_input = list(filter(lambda f : not f.suffix == '.c', self.files))
    c_files = list(filter(lambda f : f.suffix == '.c', self.files))
    absolute_sections_file = None

    absolute_sections = cast(list[AbsoluteSectionAssertion], list(filter(lambda a : isinstance(a, AbsoluteSectionAssertion), self.assertions)))
    section_addresses = None
    if absolute_sections:
      absolute_sections_file = place_all_absolute_sections(absolute_sections)
      base_cocas_input.append(absolute_sections_file)

    for opt_level in ['0', '1', '2', '3', 's']:
      binary = None
      compiled = None
      try:
        compiled = list(map(lambda f : clang_compile(f, self.clang_path, self.include_paths, opt_level), c_files))

        binary = cocas_assemble_and_link(base_cocas_input + compiled)
        connection.run_binary(binary)
        for ass in self.assertions:
          ass.check(connection.get_processor_state())

      except (CompilationError, CocasError, AssertionError) as e:
        print_error_big(f'Error in end-to-end test "{self.name}" for target "cdm-cocas" with optimization level -O{opt_level}:\n{str(e)}', file = errors_stream)
        yield False
      else:
        yield True
      finally:
        if not binary is None:
          os.remove(str(binary))
        if not compiled is None:
          for file in compiled:
            os.remove(str(file))

    if not absolute_sections_file is None:
      os.remove(str(absolute_sections_file))

  def __str__(self) -> str:
    files_string = '\n\t\t'.join(map(str, self.files))
    assertions_string = '\n\t\t'.join(map(str, self.assertions))
    return f'\033[1m{self.name}:\033[0m\n\tFiles:\n\t\t{files_string}\n\tAssertions:\n\t\t{assertions_string}'
