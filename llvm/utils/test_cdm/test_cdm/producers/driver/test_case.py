import os
from io import TextIOBase
from pathlib import Path
from dataclasses import dataclass
from test_cdm.testing_system.cocoemu import CocoemuConnection
from test_cdm.testing_system.toolchain import *
from test_cdm.testing_system.test_case import TestCase
from test_cdm.testing_system.util.errors_printing import print_error_big
from test_cdm.testing_system.configuration import Configuration
from .objs_comparing import get_objs_diff, compare_objs

@dataclass
class DriverTestCase(TestCase):
  clang: ClangCocas
  cocas: Cocas
  opt_level: str

  def run(self, connection: CocoemuConnection, errors_stream: TextIOBase) -> bool:
    ret = True
    for file in filter(lambda f: f.suffix == '.c', self.files):
      obj_from_clang: Path | None = None
      asm: Path | None = None
      obj_from_cocas: Path | None = None
      try:
        obj_from_clang = self.clang.assemble(file, self.opt_level)
        asm = self.clang.compile(file, self.opt_level)
        obj_from_cocas = self.cocas.assemble(asm)
      except (CompilationError, CocasError) as e:
        print_error_big(f'Error in {self.name}:\n{str(e)}', file = errors_stream)
        ret = False
        break
      else:
        if not compare_objs(obj_from_cocas, obj_from_clang):
          from_cocas, from_clang = get_objs_diff(obj_from_cocas, obj_from_clang)
          print_error_big(f'Error in {self.name}:\nClang driver and clang+cocas generated different output\nCocas:\n{from_cocas}\nClang:\n{from_clang}', file = errors_stream)
          ret = False
          break
      finally:
        if not obj_from_clang is None:
          os.remove(str(obj_from_clang))
        if not asm is None:
          os.remove(str(asm))
        if not obj_from_cocas is None:
          os.remove(str(obj_from_cocas))

    return ret

  def __str__(self) -> str:
    files_string = '\n\t\t'.join(map(str, self.files))
    return f'\033[1m{self.name}:\033[0m\n\t{files_string}'
