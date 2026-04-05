from pathlib import Path
from dataclasses import dataclass
from abc import ABC, abstractmethod
from enum import Enum
import subprocess
import tempfile
import os
from .configuration import Configuration


@dataclass
class CompilationError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

@dataclass
class CocasError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

@dataclass
class Clang(ABC):
  config: Configuration
  target: str

  def compile(self, filepath: Path, opt_level: str, output_path: Path | None = None) -> Path:
    try:
      if output_path is None:
        output_file = tempfile.NamedTemporaryFile(suffix = '.s', delete=False)
        output_path = Path(output_file.name)
        output_file.close()

      clang_args = [str(self.config.clang_path), '-target', self.target, '-S', f'-O{opt_level}', '-o', str(output_path)]
      for i in self.config.include_paths:
          clang_args.append('-I')
          clang_args.append(str(i))

      clang_args.append(str(filepath))
      clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

      if not clang_proc.returncode == 0:
        raise CompilationError(f"Failed when tried to compile {str(filepath)} with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
      return output_path

    except BaseException as e:
      if not output_path is None and output_path.exists():
        os.remove(str(output_path))
      raise e

  def assemble(self, filepath: Path, opt_level: str, output_path: Path | None = None) -> Path:
    try:
      if output_path is None:
        output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
        output_path = Path(output_file.name)
        output_file.close()

      clang_args = [str(self.config.clang_path), '-target', self.target, '-c', f'-O{opt_level}', '-o', str(output_path)]
      for i in self.config.include_paths:
          clang_args.append('-I')
          clang_args.append(str(i))

      clang_args.append(str(filepath))
      clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':self.config.cocas_path})

      if not clang_proc.returncode == 0:
        raise CompilationError(f"Failed when tried to assemble {str(filepath)} with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
      return output_path

    except BaseException as e:
      if not output_path is None and output_path.exists():
        os.remove(str(output_path))
      raise e

  @abstractmethod
  def link(self, files: list[Path], opt_level: str, output_path: Path | None = None, linker_script: Path | None = None) -> Path:
    pass

class ClangELF(Clang):
  def __init__(self, config: Configuration) -> None:
    super().__init__(config, "cdm")

  def link(self, files: list[Path], opt_level: str, output_path: Path | None = None, linker_script: Path | None = None) -> Path:
    try:
      if output_path is None:
        output_file = tempfile.NamedTemporaryFile(suffix = '.img', delete=False)
        output_path = Path(output_file.name)
        output_file.close()

      clang_args = [str(self.config.clang_path), '-target', self.target, f'-O{opt_level}', '-o', '-']
      for i in self.config.include_paths:
        clang_args.append('-I')
        clang_args.append(str(i))
      if not linker_script is None:
        clang_args.append('-T')
        clang_args.append(str(linker_script))
      clang_args.append('-Wl,--oformat=binary')
      for i in files:
        clang_args.append(str(i))

      # print(' '.join(clang_args), end = "\n\n")
      clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':self.config.cocas_path})

      if not clang_proc.returncode == 0:
        raise CompilationError(f"Failed when tried to link files with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")

      with output_path.open(mode = 'wt') as out:
        out.write("v2.0 raw\n")
        out.write(clang_proc.stdout.hex(sep = '\n'))

      return output_path

    except BaseException as e:
      if not output_path is None and output_path.exists():
        os.remove(str(output_path))
      raise e

class ClangCocas(Clang):
  def __init__(self, config: Configuration) -> None:
    super().__init__(config, "cdm-cocas")

  def link(self, files: list[Path], opt_level: str, output_path: Path | None = None, linker_script: Path | None = None) -> Path:
    try:
      if output_path is None:
        output_file = tempfile.NamedTemporaryFile(suffix = '.img', delete=False)
        output_path = Path(output_file.name)
        output_file.close()

      clang_args = [str(self.config.clang_path), '-target', self.target, f'-O{opt_level}', '-o', str(output_path)]
      for i in self.config.include_paths:
        clang_args.append('-I')
        clang_args.append(str(i))
      if not linker_script is None:
        clang_args.append('-T')
        clang_args.append(str(linker_script))
      for i in files:
        clang_args.append(str(i))

      # print(' '.join(clang_args), end = "\n\n")
      clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':self.config.cocas_path})

      if not clang_proc.returncode == 0:
        raise CompilationError(f"Failed when tried to link files with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")

      return output_path

    except BaseException as e:
      if not output_path is None and output_path.exists():
        os.remove(str(output_path))
      raise e
