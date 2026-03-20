from dataclasses import dataclass
from pathlib import Path

@dataclass
class Configuration:
  cocoemu_port: int
  verbose: bool
  log: bool
  search_points: list[Path]
  clang_path: Path
  llvm_objcopy_path: Path
  cocas_path: Path
  cocoemu_path: Path
  include_paths: list[Path]
  resources_path: Path
