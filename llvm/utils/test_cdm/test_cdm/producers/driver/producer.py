from pathlib import Path
from test_cdm.testing_system.test_case_producer import TestCaseProducer, TestProducingError
from test_cdm.testing_system.parse.directive import Directive
from test_cdm.testing_system.toolchain import ClangCocas, Cocas
from test_cdm.testing_system.test_case import TestCase
from test_cdm.testing_system.processor import ProcessorInfo
from test_cdm.testing_system.configuration import Configuration
from .test_case import DriverTestCase

class DriverTestCaseProducer(TestCaseProducer):
  clang: ClangCocas
  cocas: Cocas

  def __init__(self, config: Configuration, processor_info: ProcessorInfo):
    super().__init__(config, processor_info)

    self.clang = ClangCocas(config)
    self.cocas = Cocas(config)

  def produce(self, name: str, files: list[Path], args: list[str], directives: list[Directive]) -> list[TestCase]:
    ret: list[TestCase] = []
    for opt_level in ['0', '1', '2', '3', 's']:
      ret.append(DriverTestCase(f'Clang driver "{name}" with optimization level -O{opt_level}',
                                files,
                                self.clang,
                                self.cocas,
                                opt_level))
    return ret
