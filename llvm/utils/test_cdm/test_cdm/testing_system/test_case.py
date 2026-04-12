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
  include_paths: list[Path]
  clang: Clang
  assertions: list[Assertion]
  opt_level: str

  _START_OF_ABSOLUTE_SECTIONS = 0x80

  @classmethod
  def place_all_absolute_sections(cls, absolute_sections: list[AbsoluteSectionAssertion]) -> None:
    next_address = cls._START_OF_ABSOLUTE_SECTIONS

    for sec in absolute_sections:
      sec.address = next_address
      next_address += len(sec.content)

  @abstractmethod
  def produce_image(self) -> Path:
    pass

  def run(self, connection: CocoemuConnection, errors_stream: TextIOBase) -> bool:
    ret = False

    binary = None
    try:
      binary = self.produce_image()

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

  def produce_image(self) -> Path:
    absolute_sections = cast(list[AbsoluteSectionAssertion], list(filter(lambda a : isinstance(a, AbsoluteSectionAssertion), self.assertions)))
    self.place_all_absolute_sections(absolute_sections)

    absolute_sections_file = None
    try:
      files = self.files
      absolute_sections_file = self.generate_absolute_sections_file(absolute_sections)
      if not absolute_sections_file is None:
        files += [absolute_sections_file]

      return self.clang.link(files, self.include_paths, self.opt_level)
    finally:
      if not absolute_sections_file is None:
        os.remove(str(absolute_sections_file))

@dataclass
class ElfEndToEndTestCase(TestCase):
  common_linker_script: Path

  @staticmethod
  def generate_linker_script(absolute_sections: list[AbsoluteSectionAssertion]) -> Path:
    with tempfile.NamedTemporaryFile(suffix = '.ld', delete=False, mode='wt') as temp:
      temp.write("SECTIONS {\n")
      for sec in absolute_sections:
        temp.write(f"""
                    .{sec.symbol}_{sec.address} {sec.address} : {{
                        {sec.symbol} = {sec.address};
                    }}

                    """)

      temp.write("}")

      return Path(temp.name)

  @staticmethod
  def binary_to_image(file: Path) -> Path:
      with tempfile.NamedTemporaryFile(suffix = '.img', delete=False, mode='wt') as output_file, file.open(mode = 'rb') as binary:
        output_file.write("v2.0 raw\n")
        output_file.write(binary.read().hex(sep = '\n'))
        return Path(output_file.name)

  def produce_image(self) -> Path:
    absolute_sections = cast(list[AbsoluteSectionAssertion], list(filter(lambda a : isinstance(a, AbsoluteSectionAssertion), self.assertions)))
    self.place_all_absolute_sections(absolute_sections)

    linker_script = None
    binary_file = None
    try:
      linker_script = self.generate_linker_script(absolute_sections)
      binary_file = self.clang.link(self.files, self.include_paths, self.opt_level, None, [linker_script, self.common_linker_script])

      return self.binary_to_image(binary_file)
    finally:
      if not linker_script is None:
        os.remove(str(linker_script))
      if not binary_file is None:
        os.remove(str(binary_file))

