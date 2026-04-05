import re
import itertools
from pathlib import Path
from dataclasses import dataclass
from collections.abc import Generator
from test_cdm.testing_system.test_case_producer import TestCaseProducer, TestProducingError
from test_cdm.testing_system.test_case import TestCase
from test_cdm.testing_system.processor import ProcessorInfo
from test_cdm.testing_system.assertions import Assertion
from .test_parsing_error import TestParsingError
from .directive import Directive
from .directives_parser import parse_directive

_DIRECTIVE_PATTERN = re.compile("^.*?//\\s*CHECK\\s(.*)\n")
_DIRECTIVE_INNER_PATTERN = re.compile("\\s*(\\S+)\\((.*)\\)\\s*(.*)")

def iter_directives(filepath: Path) -> Generator[Directive]:
  if not filepath.is_file():
    raise ValueError("Expected regular file")

  if not filepath.suffix == '.c':
    return

  with filepath.open() as file:
    line_num = 1

    for line in file:
      m = _DIRECTIVE_PATTERN.fullmatch(line)
      if not m is None:
        m = _DIRECTIVE_INNER_PATTERN.fullmatch(m.group(1))
        if not m is None:
          yield Directive(m.group(1), m.group(2).strip().split(), m.group(3).strip().split(), line_num)
        else:
          raise TestParsingError(f'{filepath.name}:{line_num} Wrong directive format')
      line_num += 1

@dataclass
class TestsParser:
  processor_info: ProcessorInfo
  producer: TestCaseProducer

  def parse_all_test_cases(self, base_name: str, files: list[Path]) -> list[TestCase]:
    try:
      assertions: list[Assertion] = []

      for file in files:
        for directive in iter_directives(file):
            assertions.append(parse_directive(directive, self.processor_info))

      return self.producer.produce(base_name, files, assertions)
    except TestParsingError as e:
      raise TestParsingError(str(file) + ':' + str(e))
    except TestProducingError as e:
      raise TestProducingError(str(file) + ':' + str(e))

  def parse_test(self, filepath: Path) -> list[TestCase]:
    files: list[Path] = []

    if filepath.is_file():
      files.append(filepath.absolute())
    elif filepath.is_dir():
      for file in filter(lambda p: p.is_file() and not p.name == ".multi_source", filepath.iterdir()):
        files.append(file.absolute())
    else:
      raise ValueError("Expected path to regular file or directory")

    return self.parse_all_test_cases(filepath.stem, files)

  def collect_tests(self, search_point: Path) -> list[TestCase]:
    if not search_point.exists():
      raise TestParsingError(f'Failed to collect tests from "{str(search_point)}": No such file or directory')
    elif search_point.is_file() or (search_point.is_dir() and (search_point / ".multi_source").exists()):
      return self.parse_test(search_point)
    elif search_point.is_dir():
      return list(itertools.chain.from_iterable(map(lambda p: self.collect_tests(p), search_point.iterdir())))
    else:
      return []
