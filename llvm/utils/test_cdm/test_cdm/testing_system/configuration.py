from dataclasses import dataclass, field
from pathlib import Path

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
