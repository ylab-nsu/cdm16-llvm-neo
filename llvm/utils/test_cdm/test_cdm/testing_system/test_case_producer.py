from pathlib import Path
from dataclasses import dataclass
from .test_case import TestCase
from .configuration import Configuration
from .processor import ProcessorInfo
from .test_case import TestCase
from .configuration import Configuration
from .toolchain import ClangCocas, ClangELF, CompilationError
from .test_case import CocasEndToEndTestCase, ElfEndToEndTestCase
from .assertions import Assertion

@dataclass
class TestProducingError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

class TestCaseProducer:
  config: Configuration
  clang_cocas: ClangCocas
  clang_elf: ClangELF
  cocas: list[Path] = []
  elf: list[Path] = []


  def __init__(self, config: Configuration):
    self.config = config

    self.clang_elf = ClangELF(config)
    self.clang_cocas = ClangCocas(config)

    common_resources = config.resources_path / "end_to_end_common"
    elf_resources = config.resources_path / "end_to_end_elf"

    elf_build = config.resources_path / "build" / "end_to_end_elf"
    cocas_build = config.resources_path / "build" / "end_to_end_cocas"

    if not common_resources.exists():
      raise TestProducingError(f'Cannot find resources directory at {common_resources}')
    if not elf_resources.exists():
      raise TestProducingError(f'Cannot find resources directory at {elf_resources}')

    elf_build.mkdir(exist_ok=True, parents=True)
    cocas_build.mkdir(exist_ok=True, parents=True)

    try:
      for file in filter(Path.is_file, common_resources.iterdir()):
        output_path = (cocas_build / file.name).with_suffix('.o')
        if (not output_path.exists()) or (file.stat().st_mtime > output_path.stat().st_mtime):
          output_path = self.clang_cocas.assemble(file, '3', output_path)
        self.cocas.append(output_path)

        output_path = (elf_build / file.name).with_suffix('.o')
        if (not output_path.exists()) or (file.stat().st_mtime > output_path.stat().st_mtime):
          output_path = self.clang_elf.assemble(file, '3', output_path)
        self.elf.append(output_path)

      for file in filter(Path.is_file, elf_resources.iterdir()):
        output_path = (elf_build / file.name).with_suffix('.o')
        if (not output_path.exists()) or (file.stat().st_mtime > output_path.stat().st_mtime):
          output_path = self.clang_elf.assemble(file, '3', output_path)
        self.elf.append(output_path)

    except CompilationError as e:
      raise TestProducingError(str(e))

  def produce(self, name: str, files: list[Path], assertions: list[Assertion]) -> list[TestCase]:
    ret: list[TestCase] = []
    for opt_level in ['0', '1', '2', '3', 's']:
      ret.append(CocasEndToEndTestCase(f'Cocas end-to-end "{name}" with optimization level -O{opt_level}',
                                  files + self.cocas,
                                  self.clang_cocas,
                                  assertions,
                                  opt_level))
      ret.append(ElfEndToEndTestCase(f'Cocas-less end-to-end "{name}" with optimization level -O{opt_level}',
                                  files + self.elf,
                                  self.clang_elf,
                                  assertions,
                                  opt_level))

    return ret
