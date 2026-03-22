from pathlib import Path
from testing_system.test_case_producer import TestCaseProducer, TestProducingError
from testing_system.parse.directive import Directive
from testing_system.test_case import TestCase
from testing_system.configuration import Configuration
from testing_system.processor import ProcessorInfo
from testing_system.toolchain import clang_compile_and_assemble, CompilationError, Target
from .end_to_end_test_case import EndToEndTestCase
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
      ret.append(EndToEndTestCase(f'Cocas end-to-end "{name}" with optimization level -O{opt_level}',
                                  files + self.cocas,
                                  list(map(lambda d: parse_directive(d, self.processor_info), directives)),
                                  opt_level,
                                  Target.CDM_COCAS))
      ret.append(EndToEndTestCase(f'Cocas-less end-to-end "{name}" with optimization level -O{opt_level}',
                                  files + self.elf,
                                  list(map(lambda d: parse_directive(d, self.processor_info), directives)),
                                  opt_level,
                                  Target.CDM_ELF))

    return ret
