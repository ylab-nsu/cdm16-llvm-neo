from pathlib import Path
from abc import ABC, abstractmethod
from dataclasses import dataclass
from io import TextIOBase
from .cocoemu import CocoemuConnection
from .configuration import Configuration

@dataclass
class TestCase(ABC):
 name: str
 files: list[Path]

 @abstractmethod
 def run(self, connection: CocoemuConnection, config: Configuration, errors_stream: TextIOBase) -> bool:
   pass
