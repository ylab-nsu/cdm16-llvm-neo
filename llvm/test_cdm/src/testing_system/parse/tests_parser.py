import re
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
            curr_producer_args = directive.head[1:]
            directives = []
          except KeyError:
            raise TestParsingError(f'{directive.line} Unknown tests producer: {curr_producer}')
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
    for file in filter(Path.is_file, filepath.iterdir()):
      files.append(file.absolute())
  else:
    raise ValueError("Expected path to regular file or directory")

  return parse_all_test_cases(filepath.stem, files, producers)

def parse_all_tests(test_dir: Path, producers: dict[str, TestCaseProducer]) -> list[TestCase]:
  tests: list[TestCase] = []
  for test in filter((lambda f : f.is_file() or f.is_dir()), test_dir.iterdir()):
    tests.extend(parse_test(test, producers))

  return tests
