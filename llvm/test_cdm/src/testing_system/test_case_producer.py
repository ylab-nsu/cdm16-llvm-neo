from abc import ABC, abstractmethod
from pathlib import Path
from dataclasses import dataclass
from .parse.directive import Directive
from .test_case import TestCase
from .configuration import Configuration
from .processor import ProcessorInfo

@dataclass
class TestProducingError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

@dataclass
class TestCaseProducer(ABC):
  config: Configuration
  processor_info: ProcessorInfo

  @abstractmethod
  def produce(self, name: str, files: list[Path], args: list[str], directives: list[Directive]) -> list[TestCase]:
    pass
