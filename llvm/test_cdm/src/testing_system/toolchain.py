from pathlib import Path
from dataclasses import dataclass
from enum import StrEnum
import subprocess
import tempfile
import os
import sys
from .configuration import Configuration

class Target(StrEnum):
  CDM_COCAS = "cdm-cocas"
  CDM_ELF = "cdm"

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

def clang_compile_assemble_link(files: list[Path], target: Target, config: Configuration, opt_level: str, output_path: Path | None = None) -> Path:
  if output_path is None:
    output_file = tempfile.NamedTemporaryFile(suffix = '.img', delete=False)
    output_path = Path(output_file.name)
    output_file.close()

  clang_args = [str(config.clang_path), '-target', str(target), f'-O{opt_level}', '-o', str(output_path)]
  for i in config.include_paths:
      clang_args.append('-I')
      clang_args.append(str(i))
  for i in files:
      clang_args.append(str(i))

  #print(' '.join(clang_args), end = "\n\n")
  clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':config.cocas_path})

  if not clang_proc.returncode == 0:
    if os.path.exists(str(output_path)):
      os.remove(str(output_path))
    raise CompilationError(f"Failed when tried to compile files with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
  return output_path

def clang_compile_and_assemble(filepath: Path, target: Target, config: Configuration, opt_level: str, output_path: Path | None = None) -> Path:
  if output_path is None:
    output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
    output_path = Path(output_file.name)
    output_file.close()

  clang_args = [str(config.clang_path), '-target', str(target), '-c', f'-O{opt_level}', '-o', str(output_path)]
  for i in config.include_paths:
      clang_args.append('-I')
      clang_args.append(str(i))

  clang_args.append(str(filepath))
  #print(' '.join(clang_args), end = "\n\n")
  clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':config.cocas_path})

  if not clang_proc.returncode == 0:
    if os.path.exists(str(output_path)):
      os.remove(str(output_path))
    raise CompilationError(f"Failed when tried to compile and assemble {str(filepath)} with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
  return output_path

def clang_compile(filepath: Path, target: Target, config: Configuration, opt_level: str, output_path: Path | None = None) -> Path:
  if output_path is None:
    output_file = tempfile.NamedTemporaryFile(suffix = '.s', delete=False)
    output_path = Path(output_file.name)
    output_file.close()

  clang_args = [str(config.clang_path), '-target', str(target), '-S', f'-O{opt_level}', '-o', str(output_path)]
  for i in config.include_paths:
      clang_args.append('-I')
      clang_args.append(str(i))

  clang_args.append(str(filepath))
  #print(' '.join(clang_args), end = "\n\n")
  clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  if not clang_proc.returncode == 0:
    if os.path.exists(str(output_path)):
      os.remove(str(output_path))
    raise CompilationError(f"Failed when tried to compile {str(filepath)} with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
  return output_path

def cocas_assemble(filepath: Path, config: Configuration, output_path: Path | None = None) -> Path:
  if output_path is None:
    output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
    output_path = Path(output_file.name)
    output_file.close()

  #print(' '.join([".venv/bin/cocas","-t","cdm16","-o", str(output_path), ] + [str(i) for i in cocas_input]))

  cocas_proc = subprocess.run([
                                str(config.cocas_path),
                                "-t",
                                "cdm16",
                                "-c",
                                "-o",
                                str(output_path),
                                str(filepath)
                              ],
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE)
  if not cocas_proc.returncode == 0:
    if os.path.exists(str(output_path)):
      os.remove(str(output_path))
    raise CocasError(f"Failed when tried to assemble {str(filepath)} with return code {cocas_proc.returncode}\nStdout:\n{cocas_proc.stdout.decode()}\nStderr:\n{cocas_proc.stderr.decode()}")

  return output_path

def cocas_assemble_and_link(cocas_input: list[Path], config: Configuration, output_path: Path | None = None) -> Path:
  if output_path is None:
    output_file = tempfile.NamedTemporaryFile(suffix = '.img', delete=False)
    output_path = Path(output_file.name)
    output_file.close()

  cocas_proc = subprocess.run([
                                str(config.cocas_path),
                                "-t",
                                "cdm16",
                                "-o",
                                str(output_path),
                              ] + [str(i) for i in cocas_input],
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE)
  if not cocas_proc.returncode == 0:
    if os.path.exists(str(output_path)):
      os.remove(str(output_path))
    raise CocasError(f"Failed when tried to assemble and link {' '.join(map(str, cocas_input))} with return code {cocas_proc.returncode}\nStdout:\n{cocas_proc.stdout.decode()}\nStderr:\n{cocas_proc.stderr.decode()}")

  return output_path
