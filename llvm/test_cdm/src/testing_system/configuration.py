from dataclasses import dataclass
from pathlib import Path

@dataclass
class Configuration:
  cocoemu_port: int
  verbose: bool
  test_dir: Path
  clang_path: Path
  include_paths: list[Path]
  resources_path: Path
