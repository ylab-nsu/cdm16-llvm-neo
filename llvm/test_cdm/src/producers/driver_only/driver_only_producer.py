from pathlib import Path
from testing_system.test_case_producer import TestCaseProducer, TestProducingError
from testing_system.parse.directive import Directive
from testing_system.test_case import TestCase
from testing_system.configuration import Configuration
from .driver_only_test_case import DriverOnlyTestCase

class DriverOnlyTestCaseProducer(TestCaseProducer):
  def produce(self, name: str, files: list[Path], args: list[str], directives: list[Directive]) -> list[TestCase]:
    return [DriverOnlyTestCase(f'Clang driver "{name}" with optimization level -O{opt_level}', files, self.config.clang_path, self.config.include_paths, opt_level) for opt_level in ['0', '1', '2', '3', 's']]
