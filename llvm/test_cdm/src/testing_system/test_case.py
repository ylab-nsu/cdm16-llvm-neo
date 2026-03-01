from pathlib import Path
from abc import ABC, abstractmethod
from dataclasses import dataclass
from collections.abc import Generator
from io import TextIOBase
from testing_system.parse.directive import Directive
from .cocoemu import CocoemuConnection
from .processor import ProcessorInfo
from .configuration import Configuration

@dataclass
class TestCase(ABC):
 name: str
 files: list[Path]

 @abstractmethod
 def run_tests(self, connection: CocoemuConnection, errors_stream: TextIOBase) -> Generator[bool]:
   pass
