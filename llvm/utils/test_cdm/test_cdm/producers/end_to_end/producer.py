from pathlib import Path
from test_cdm.testing_system.test_case_producer import TestCaseProducer, TestProducingError
from test_cdm.testing_system.parse.directive import Directive
from test_cdm.testing_system.test_case import TestCase
from test_cdm.testing_system.configuration import Configuration
from test_cdm.testing_system.processor import ProcessorInfo
from test_cdm.testing_system.toolchain import clang_compile_and_assemble, CompilationError, Target
from .test_case import CocasEndToEndTestCase, ElfEndToEndTestCase
from .parsing import parse_directive

class EndToEndTestCaseProducer(TestCaseProducer):
  cocas: list[Path] = []
  elf: list[Path] = []

  def __init__(self, config: Configuration, processor_info: ProcessorInfo):
    self.config = config
    self.processor_info = processor_info

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
          output_path = clang_compile_and_assemble(file, Target.CDM_COCAS, self.config, '3', output_path)
        self.cocas.append(output_path)

        output_path = (elf_build / file.name).with_suffix('.o')
        if (not output_path.exists()) or (file.stat().st_mtime > output_path.stat().st_mtime):
          output_path = clang_compile_and_assemble(file, Target.CDM_ELF, self.config, '3', output_path)
        self.elf.append(output_path)

      for file in filter(Path.is_file, elf_resources.iterdir()):
        output_path = (elf_build / file.name).with_suffix('.o')
        if (not output_path.exists()) or (file.stat().st_mtime > output_path.stat().st_mtime):
          output_path = clang_compile_and_assemble(file, Target.CDM_ELF, self.config, '3', output_path)
        self.elf.append(output_path)

    except CompilationError as e:
      raise TestProducingError(str(e))

  def produce(self, name: str, files: list[Path], args: list[str], directives: list[Directive]) -> list[TestCase]:
    ret: list[TestCase] = []
    for opt_level in ['0', '1', '2', '3', 's']:
      ret.append(CocasEndToEndTestCase(f'Cocas end-to-end "{name}" with optimization level -O{opt_level}',
                                  files + self.cocas,
                                  list(map(lambda d: parse_directive(d, self.processor_info), directives)),
                                  opt_level))
      ret.append(ElfEndToEndTestCase(f'Cocas-less end-to-end "{name}" with optimization level -O{opt_level}',
                                  files + self.elf,
                                  list(map(lambda d: parse_directive(d, self.processor_info), directives)),
                                  opt_level))

    return ret
