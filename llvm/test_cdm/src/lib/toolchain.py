from pathlib import Path
import subprocess
import tempfile
import os
import sys
from .exceptions import CompilationError
from .exceptions import CocasError

_START_OF_ABSOLUTE_SECTIONS = 0x80

def place_all_absolute_sections(absolute_sections):
  next_address = _START_OF_ABSOLUTE_SECTIONS

  section_addresses = {}

  with tempfile.NamedTemporaryFile(suffix = '.asm', delete=False, mode='wt') as temp:
    for sec in absolute_sections:
      section_addresses[sec.symbol] = next_address
      temp.write(f"asect {next_address}\n{sec.symbol}> ds {len(sec.content)}\n")
      next_address += len(sec.content)
    temp.write('end.\n')
    return (temp.name, section_addresses)

def clang_compile(filepath, clang_path, include_paths, opt_level):
  output_file = tempfile.NamedTemporaryFile(suffix = '.asm', delete=False)
  output_path = Path(output_file.name) 
  output_file.close()

  clang_args = [str(clang_path), '-target', 'cdm', '-S', f'-O{opt_level}', '-o', str(output_path)]
  for i in include_paths:
      clang_args.append('-I')
      clang_args.append(str(i))

  clang_args.append(str(filepath))
  clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  # print(' '.join(clang_args), end = "\n\n")

  if not clang_proc.returncode == 0:
    os.remove(str(output_path))
    raise CompilationError(f"Failed when tried to compile {str(filepath)} with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
  return output_path

def clang_compile_and_assemble(filepath, clang_path, include_paths, opt_level):
  output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
  output_path = Path(output_file.name) 
  output_file.close()

  clang_args = [str(clang_path), '-target', 'cdm', '-c', f'-O{opt_level}', '-o', str(output_path)]
  for i in include_paths:
      clang_args.append('-I')
      clang_args.append(str(i))

  clang_args.append(str(filepath))
  clang_proc = subprocess.run(clang_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env = {'COCAS':f'{sys.prefix}/bin/cocas'})
  # print(' '.join(clang_args), end = "\n\n")

  if not clang_proc.returncode == 0:
    os.remove(str(output_path))
    raise CompilationError(f"Failed when tried to compile and assemble {str(filepath)} with return code {clang_proc.returncode}\nStdout:\n{clang_proc.stdout.decode()}\nStderr:\n{clang_proc.stderr.decode()}")
  return output_path

def cocas_assemble(filepath):
  output_file = tempfile.NamedTemporaryFile(suffix = '.obj', delete=False)
  output_path = Path(output_file.name) 
  output_file.close()

  #print(' '.join([".venv/bin/cocas","-t","cdm16","-o", str(output_path), ] + [str(i) for i in cocas_input]))

  cocas_proc = subprocess.run([
                                "bin/cocas",
                                "-t",
                                "cdm16",
                                "-c",
                                "-o",
                                str(output_path),
                                str(filepath)
                              ],
                              cwd = sys.prefix,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE)
  if not cocas_proc.returncode == 0:
    os.remove(str(output_path))
    raise CocasError(f"Failed when tried to assemble {str(filepath)} with return code {cocas_proc.returncode}\nStdout:\n{cocas_proc.stdout.decode()}\nStderr:\n{cocas_proc.stderr.decode()}")

  return output_path

def cocas_assemble_and_link(cocas_input):
  output_file = tempfile.NamedTemporaryFile(suffix = '.img', delete=False)
  output_path = Path(output_file.name) 
  output_file.close()

  #print(' '.join([".venv/bin/cocas","-t","cdm16","-o", str(output_path), ] + [str(i) for i in cocas_input]))

  cocas_proc = subprocess.run([
                                "bin/cocas",
                                "-t",
                                "cdm16",
                                "-o",
                                str(output_path),
                              ] + [str(i) for i in cocas_input],
                              cwd = sys.prefix,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE)
  if not cocas_proc.returncode == 0:
    os.remove(str(output_path))
    raise CocasError(f"Failed when tried to assemble and link {' '.join(map(str, cocas_input))} with return code {cocas_proc.returncode}\nStdout:\n{cocas_proc.stdout.decode()}\nStderr:\n{cocas_proc.stderr.decode()}")

  return output_path
