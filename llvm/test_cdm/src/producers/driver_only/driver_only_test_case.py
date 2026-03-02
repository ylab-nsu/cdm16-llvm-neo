import os
from io import TextIOBase
from pathlib import Path
from dataclasses import dataclass
from collections.abc import Generator
from testing_system.cocoemu import CocoemuConnection
from testing_system.toolchain import *
from testing_system.test_case import TestCase
from testing_system.util.errors_printing import print_error_big
from .objs_comparing import get_objs_diff, compare_objs

@dataclass
class DriverOnlyTestCase(TestCase):
  clang_path: Path
  include_paths: list[Path]

  def run_tests(self, connection: CocoemuConnection, errors_stream: TextIOBase) -> Generator[bool]:
    for file in filter(lambda f: f.suffix == '.c', self.files):
      for opt_level in ['0', '1', '2', '3', 's']:
        obj_from_clang: Path
        asm: Path
        obj_from_cocas: Path
        try:
          obj_from_clang = clang_compile_and_assemble(file, self.clang_path, self.include_paths, opt_level)
          asm = clang_compile(file, self.clang_path, self.include_paths, opt_level)
          obj_from_cocas = cocas_assemble(asm)
        except (CompilationError, CocasError) as e:
          print_error_big(f'Error in clang driver test {self.name} with optimization level -O{opt_level}:\n{str(e)}', file = errors_stream)
          yield False
        else:
          if not compare_objs(obj_from_cocas, obj_from_clang):
            from_cocas, from_clang = get_objs_diff(obj_from_cocas, obj_from_clang)
            print_error_big(f'Error in clang driver test {self.name} with optimization level -O{opt_level}:\nClang driver and clang+cocas generated different output\nCocas:\n{from_cocas}\nClang:\n{from_clang}', file = errors_stream)
            yield False
          else:
            yield True
        finally:
          if not obj_from_clang is None:
            os.remove(str(obj_from_clang))
          if not asm is None:
            os.remove(str(asm))
          if not obj_from_cocas is None:
            os.remove(str(obj_from_cocas))

  def __str__(self) -> str:
    files_string = '\n\t\t'.join(map(str, self.files))
    return f'\033[1mDriver only "{self.name}":\033[0m\n\t{files_string}'
