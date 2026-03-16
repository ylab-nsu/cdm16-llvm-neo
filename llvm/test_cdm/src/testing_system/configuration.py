from dataclasses import dataclass
from pathlib import Path

@dataclass
class Configuration:
  cocoemu_port: int
  verbose: bool
  log: bool
  tests_to_run: list[Path]
  clang_path: Path
  cocas_path: Path
  include_paths: list[Path]
  resources_path: Path
