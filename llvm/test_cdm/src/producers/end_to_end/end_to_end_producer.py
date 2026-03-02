from testing_system.test_case_producer import TestCaseProducer, TestProducingError
from pathlib import Path
from testing_system.parse.directive import Directive
from testing_system.test_case import TestCase
from testing_system.configuration import Configuration
from testing_system.processor import ProcessorInfo
from testing_system.toolchain import clang_compile_and_assemble, CompilationError
from .end_to_end_test_case import CocasEndToEndTestCase
from .parsing import parse_directive

class EndToEndTestCaseProducer(TestCaseProducer):
  common: list[Path] = []

  def __init__(self, config: Configuration):
    self.config = config

    resources = config.resources_path / "end_to_end_cocas"
    build = resources / "build"

    if not resources.exists():
      raise TestProducingError(f'Cannot find resources directory at {resources}')

    build.mkdir(exist_ok=True)

    try:
      for file in filter(Path.is_file, resources.iterdir()):
        output_path = (build / file.name).with_suffix('.obj')
        if (not output_path.exists()) or (file.stat().st_mtime > output_path.stat().st_mtime):
          output_path = clang_compile_and_assemble(file, self.config.clang_path, self.config.include_paths, '3', output_path)
        self.common.append(output_path)
    except CompilationError as e:
      raise TestProducingError(str(e))

  def produce(self, name: str, files: list[Path], args: list[str], directives: list[Directive]) -> list[TestCase]:
    ret: list[TestCase] = []
    if 'cocas' in args:
      ret.append(CocasEndToEndTestCase(name, files + self.common, list(map(lambda d: parse_directive(d, self.processor_info), directives)), self.config.clang_path, self.config.include_paths))
    if 'elf' in args:
      pass

    return ret
