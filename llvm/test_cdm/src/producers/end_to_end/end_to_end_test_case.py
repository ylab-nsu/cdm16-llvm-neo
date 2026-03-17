import os
import tempfile
from typing import cast
from io import TextIOBase
from dataclasses import dataclass
from pathlib import Path
from testing_system.test_case import TestCase
from testing_system.parse.directive import Directive
from testing_system.toolchain import *
from testing_system.cocoemu import CocoemuConnection
from testing_system.util.errors_printing import print_error_big
from .assertions import Assertion, AbsoluteSectionAssertion

_START_OF_ABSOLUTE_SECTIONS = 0x80

# Returns assembly file for cocas target
# And linker script for cocas-less target
def place_all_absolute_sections(absolute_sections: list[AbsoluteSectionAssertion]) -> None:
  next_address = _START_OF_ABSOLUTE_SECTIONS

  for sec in absolute_sections:
    sec.address = next_address
    next_address += len(sec.content)

def generate_absolute_sections_file(absolute_sections: list[AbsoluteSectionAssertion]) -> Path | None:
  if not absolute_sections:
    return None
  with tempfile.NamedTemporaryFile(suffix = '.asm', delete=False, mode='wt') as temp:
    for sec in absolute_sections:
      temp.write(f"asect {sec.address}\n{sec.symbol}> ds {len(sec.content)}\n")
    temp.write('end.\n')
    return Path(temp.name)

def generate_linker_script(absolute_sections: list[AbsoluteSectionAssertion]) -> Path:
  with tempfile.NamedTemporaryFile(suffix = '.ld', delete=False, mode='wt') as temp:
    temp.write("""
               SECTIONS
               {
                   .isr_vector : {
                       *(.isr_vector)
                   }

               """)
    for sec in absolute_sections:
      temp.write(f"""
                  . = {sec.address};
                  .{sec.symbol}_{sec.address} : {{
                      {sec.symbol} = .;
                  }}

                  """)

    temp.write("""
               .text : ALIGN(2) {
                 *(.text*)
                 *(.rodata*)
               }

               .data : ALIGN(2){
                 *(.data*)
               }

               .bss : ALIGN(2){
                 *(.bss*)
                 *(COMMON)
               }

               }
               """)

    return Path(temp.name)


@dataclass
class EndToEndTestCase(TestCase):
  assertions: list[Assertion]
  opt_level: str
  target: Target

  def run(self, connection: CocoemuConnection, config: Configuration, errors_stream: TextIOBase) -> bool:
    ret = False

    absolute_sections = cast(list[AbsoluteSectionAssertion], list(filter(lambda a : isinstance(a, AbsoluteSectionAssertion), self.assertions)))
    place_all_absolute_sections(absolute_sections)

    absolute_sections_file = None
    linker_script = None
    if self.target == Target.CDM_COCAS:
      absolute_sections_file = generate_absolute_sections_file(absolute_sections)
      if not absolute_sections_file is None:
        self.files += [absolute_sections_file]
    elif self.target == Target.CDM_ELF:
      linker_script = generate_linker_script(absolute_sections)

    binary = None
    try:
      if self.target == Target.CDM_COCAS:
        binary = clang_compile_assemble_link(self.files, self.target, config, self.opt_level)
      elif self.target == Target.CDM_ELF:
        binary = clang_compile_assemble_link(self.files, self.target, config, self.opt_level, None, linker_script)

      connection.run_binary(binary)
      for ass in self.assertions:
        ass.check(connection.get_processor_state())

    except (CompilationError, CocasError, AssertionError) as e:
      print_error_big(f'Error in {self.name}:\n{str(e)}', file = errors_stream)
    else:
      ret = True
    finally:
      if not binary is None:
        os.remove(str(binary))
      if not absolute_sections_file is None:
        os.remove(str(absolute_sections_file))
      if not linker_script is None:
        os.remove(str(linker_script))

    return ret

  def __str__(self) -> str:
    files_string = '\n\t\t'.join(map(str, self.files))
    assertions_string = '\n\t\t'.join(map(str, self.assertions))
    return f'\033[1m{self.name}:\033[0m\n\tFiles:\n\t\t{files_string}\n\tAssertions:\n\t\t{assertions_string}'
