from pathlib import Path
from dataclasses import dataclass
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
class Clang:
  config: Configuration
  target: str
  _TIMEOUT: int = 60 * 5 # 5 min

  def compile(self, filepath: Path, include_paths: list[Path], opt_level: str, output_path: Path | None = None) -> Path:
    try:
      if output_path is None:
        output_file = tempfile.NamedTemporaryFile(suffix = '.s', delete=False)
        output_path = Path(output_file.name)
        output_file.close()

      clang_args = [str(self.config.clang_path), '-target', self.target, '-nostartfiles', '-ffreestanding', '-S', f'-O{opt_level}', '-o', str(output_path)]
      for i in include_paths:
          clang_args.append('-I')
          clang_args.append(str(i))

      clang_args.append(str(filepath))
      try:
        clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout = self._TIMEOUT)
      except subprocess.TimeoutExpired:
        raise TimeoutError()

      if not clang_proc.returncode == 0:
        clang_invokation = ' '.join(clang_args)
        raise CompilationError(f"Failed when tried to compile {str(filepath)} with return code {clang_proc.returncode}\nInvokation: {clang_invokation}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
      return output_path

    except BaseException as e:
      if not output_path is None and output_path.exists():
        os.remove(str(output_path))
      raise e

  def assemble(self, filepath: Path, include_paths: list[Path], opt_level: str, output_path: Path | None = None) -> Path:
    try:
      if output_path is None:
        output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
        output_path = Path(output_file.name)
        output_file.close()

      clang_args = [str(self.config.clang_path), '-target', self.target, '-nostartfiles', '-ffreestanding', '-c', f'-O{opt_level}', '-o', str(output_path)]
      for i in include_paths:
          clang_args.append('-I')
          clang_args.append(str(i))

      clang_args.append(str(filepath))
      try:
        clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':str(self.config.cocas_path)}, timeout = self._TIMEOUT)
      except subprocess.TimeoutExpired:
        raise TimeoutError()

      if not clang_proc.returncode == 0:
        clang_invokation = ' '.join(clang_args)
        raise CompilationError(f"Failed when tried to assemble {str(filepath)} with return code {clang_proc.returncode}\nInvokation: {clang_invokation}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
      return output_path

    except BaseException as e:
      if not output_path is None and output_path.exists():
        os.remove(str(output_path))
      raise e

  def link(self, files: list[Path], include_paths: list[Path], opt_level: str, output_path: Path | None = None, linker_script: list[Path] | None = None) -> Path:
    try:
      if output_path is None:
        output_file = tempfile.NamedTemporaryFile(suffix = '.img', delete=False)
        output_path = Path(output_file.name)
        output_file.close()

      clang_args = [str(self.config.clang_path), '-target', self.target, '-nostartfiles', '-ffreestanding', f'-O{opt_level}', '-o', str(output_path)]
      for i in include_paths:
        clang_args.append('-I')
        clang_args.append(str(i))
      if not linker_script is None:
        for script in linker_script:
          clang_args.append('-T')
          clang_args.append(str(script))
      for i in files:
        clang_args.append(str(i))

      # print(' '.join(clang_args), end = "\n\n")
      try:
        clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':str(self.config.cocas_path)}, timeout = self._TIMEOUT)
      except subprocess.TimeoutExpired:
        raise TimeoutError()


      if not clang_proc.returncode == 0:
        clang_invokation = ' '.join(clang_args)
        raise CompilationError(f"Failed when tried to link files with return code {clang_proc.returncode}\nInvokation: {clang_invokation}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")

      return output_path

    except BaseException as e:
      if not output_path is None and output_path.exists():
        os.remove(str(output_path))
      raise e

