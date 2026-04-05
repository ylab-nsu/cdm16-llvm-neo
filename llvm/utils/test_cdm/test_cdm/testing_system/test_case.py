import os
import tempfile
from typing import cast
from pathlib import Path
from abc import ABC, abstractmethod
from dataclasses import dataclass
from io import TextIOBase
from test_cdm.testing_system.util.errors_printing import print_error_big
from .configuration import Configuration
from .toolchain import Clang, CompilationError, CocasError
from .cocoemu import CocoemuConnection
from .assertions import Assertion, AbsoluteSectionAssertion

@dataclass
class TestCase(ABC):
  name: str
  files: list[Path]
  clang: Clang
  assertions: list[Assertion]
  opt_level: str

  _START_OF_ABSOLUTE_SECTIONS = 0x80

  # Returns assembly file for cocas target
  # And linker script for cocas-less target
  @classmethod
  def place_all_absolute_sections(cls, absolute_sections: list[AbsoluteSectionAssertion]) -> None:
    next_address = cls._START_OF_ABSOLUTE_SECTIONS

    for sec in absolute_sections:
      sec.address = next_address
      next_address += len(sec.content)

  @abstractmethod
  def produce_binary(self, files: list[Path], opt_level: str, absolute_sections: list[AbsoluteSectionAssertion]) -> Path:
    pass

  def run(self, connection: CocoemuConnection, errors_stream: TextIOBase) -> bool:
    ret = False

    absolute_sections = cast(list[AbsoluteSectionAssertion], list(filter(lambda a : isinstance(a, AbsoluteSectionAssertion), self.assertions)))
    self.place_all_absolute_sections(absolute_sections)

    binary = None
    try:
      binary = self.produce_binary(self.files, self.opt_level, absolute_sections)

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

    return ret

  def __str__(self) -> str:
    files_string = '\n\t\t'.join(map(str, self.files))
    assertions_string = '\n\t\t'.join(map(str, self.assertions))
    return f'\033[1m{self.name}:\033[0m\n\tFiles:\n\t\t{files_string}\n\tAssertions:\n\t\t{assertions_string}'


class CocasEndToEndTestCase(TestCase):
  @staticmethod
  def generate_absolute_sections_file(absolute_sections: list[AbsoluteSectionAssertion]) -> Path | None:
    if not absolute_sections:
      return None
    with tempfile.NamedTemporaryFile(suffix = '.asm', delete=False, mode='wt') as temp:
      for sec in absolute_sections:
        temp.write(f"asect {sec.address}\n{sec.symbol}> ds {len(sec.content)}\n")
      temp.write('end.\n')
      return Path(temp.name)

  def produce_binary(self, files: list[Path], opt_level: str, absolute_sections: list[AbsoluteSectionAssertion]) -> Path:
    absolute_sections_file = None
    try:
      absolute_sections_file = self.generate_absolute_sections_file(absolute_sections)
      if not absolute_sections_file is None:
        files += [absolute_sections_file]

      return self.clang.link(files, opt_level)
    finally:
      if not absolute_sections_file is None:
        os.remove(str(absolute_sections_file))

class ElfEndToEndTestCase(TestCase):
  @staticmethod
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

  def produce_binary(self, files: list[Path], opt_level: str, absolute_sections: list[AbsoluteSectionAssertion]) -> Path:
    linker_script = None
    try:
      linker_script = self.generate_linker_script(absolute_sections)
      return self.clang.link(files, opt_level, None, linker_script)
    finally:
      if not linker_script is None:
        os.remove(str(linker_script))

