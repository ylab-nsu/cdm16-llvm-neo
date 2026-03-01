from pathlib import Path
from dataclasses import dataclass
import subprocess
import tempfile
import os
import sys

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

def clang_compile(filepath: Path, clang_path: Path, include_paths: list[Path], opt_level: str) -> Path:
  output_file = tempfile.NamedTemporaryFile(suffix = '.asm', delete=False)
  output_path = Path(output_file.name)
  output_file.close()

  clang_args = [str(clang_path), '-target', 'cdm-cocas', '-S', f'-O{opt_level}', '-o', str(output_path)]
  for i in include_paths:
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

def clang_compile_and_assemble(filepath: Path, clang_path: Path, include_paths: list[Path], opt_level: str, output_path: Path | None = None) -> Path:
  if output_path is None:
    output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
    output_path = Path(output_file.name)
    output_file.close()

  clang_args = [str(clang_path), '-target', 'cdm-cocas', '-c', f'-O{opt_level}', '-o', str(output_path)]
  for i in include_paths:
      clang_args.append('-I')
      clang_args.append(str(i))

  clang_args.append(str(filepath))
  clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':f'{os.path.dirname(sys.executable)}/cocas'})
  # print(' '.join(clang_args), end = "\n\n")

  if not clang_proc.returncode == 0:
    if os.path.exists(str(output_path)):
      os.remove(str(output_path))
    raise CompilationError(f"Failed when tried to compile and assemble {str(filepath)} with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
  return output_path

def cocas_assemble(filepath: Path) -> Path:
  output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
  output_path = Path(output_file.name)
  output_file.close()

  cocas_proc = subprocess.run([
                                f"{os.path.dirname(sys.executable)}/cocas",
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

def cocas_assemble_and_link(cocas_input: list[Path]) -> Path:
  output_file = tempfile.NamedTemporaryFile(suffix = '.img', delete=False)
  output_path = Path(output_file.name)
  output_file.close()

  cocas_proc = subprocess.run([
                                f"{os.path.dirname(sys.executable)}/cocas",
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
