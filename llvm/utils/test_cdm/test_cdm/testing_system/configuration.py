import itertools
from dataclasses import dataclass, field
from pathlib import Path

@dataclass
class InvalidConfigurationError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

@dataclass
class Configuration:
  clang_path: Path
  cocas_path: Path
  cocoemu_path: Path
  resources_path: Path
  cocoemu_port: int = 7001
  verbose: bool = False
  log: bool = False
  search_points: list[Path] = field(default_factory=list)
  include_paths: list[Path] = field(default_factory=list)

  def verify(self) -> None:
    for path in itertools.chain([self.clang_path, self.cocas_path, self.cocoemu_path, self.resources_path], self.include_paths):
      if not path.exists():
        raise InvalidConfigurationError(f'File or directory "{str(path)}" doesn\'t exist')
