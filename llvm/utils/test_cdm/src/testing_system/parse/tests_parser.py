import re
import itertools
from pathlib import Path
from dataclasses import dataclass
from collections.abc import Generator
from testing_system.test_case_producer import TestCaseProducer, TestProducingError
from testing_system.test_case import TestCase
from .directive import Directive

_DIRECTIVE_PATTERN = re.compile("^.*?//\\s*CHECK\\s(.*)\n")
_DIRECTIVE_INNER_PATTERN = re.compile("\\s*(\\S+)\\((.*)\\)\\s*(.*)")

@dataclass
class TestParsingError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

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

def parse_all_test_cases(base_name: str, files: list[Path], producers: dict[str, TestCaseProducer]) -> list[TestCase]:
  test_cases: list[TestCase] = []

  for file in files:
    curr_producer: TestCaseProducer | None = None
    curr_producer_args: list[str] = []
    directives: list[Directive] = []

    try:
      for directive in iter_directives(file):
        if directive.name == "prod":
          if not curr_producer is None:
            test_cases.extend(curr_producer.produce(base_name, files, curr_producer_args, directives))

          if len(directive.head) == 0:
            raise TestParsingError(f'{directive.line} Expected at least 1 argument in directive head, got {len(directive.head)}')
          try:
            curr_producer = producers[directive.head[0]]
            curr_producer_args = directive.body
            directives = []
          except KeyError:
            raise TestParsingError(f'{directive.line} Unknown tests producer: {directive.head[0]}')
        else:
          directives.append(directive)

      if not curr_producer is None:
        test_cases.extend(curr_producer.produce(base_name, files, curr_producer_args, directives))
    except TestParsingError as e:
      raise TestParsingError(str(file) + ':' + str(e))
    except TestProducingError as e:
      raise TestProducingError(str(file) + ':' + str(e))

  return test_cases

def parse_test(filepath: Path, producers: dict[str, TestCaseProducer]) -> list[TestCase]:
  files: list[Path] = []

  if filepath.is_file():
    files.append(filepath.absolute())
  elif filepath.is_dir():
    for file in filter(lambda p: p.is_file() and not p.name == ".multi_source", filepath.iterdir()):
      files.append(file.absolute())
  else:
    raise ValueError("Expected path to regular file or directory")

  return parse_all_test_cases(filepath.stem, files, producers)

def collect_tests(search_point: Path, producers: dict[str, TestCaseProducer]) -> list[TestCase]:
  if search_point.is_file() or (search_point.is_dir() and (search_point / ".multi_source").exists()):
    return parse_test(search_point, producers)
  elif search_point.is_dir():
    return list(itertools.chain.from_iterable(map(lambda p: collect_tests(p, producers), search_point.iterdir())))
  else:
    return []
