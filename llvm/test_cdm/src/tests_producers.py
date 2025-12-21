import os
from lib.exceptions import CompilationError
from lib.exceptions import CocasError
from lib.exceptions import AssertionsError
from lib.util.errors_printing import print_error_big
from lib.toolchain import *
from lib.assertions import check_assertions
from lib.cocoemu import run_binary
from lib.tests_producer import tests_producer
from lib.structs import AbsoluteSectionAssertion
from lib.util.objs_comparing import compare_objs
from lib.util.objs_comparing import get_objs_diff

@tests_producer
def run_end_to_end_tests(ws, base_test, processor_info, clang_path, include_paths, errors_stream):

  base_cocas_input = list(filter(lambda f : not f.suffix == '.c', base_test.files))
  c_files = list(filter(lambda f : f.suffix == '.c', base_test.files))
  absolute_sections_file = None

  absolute_sections = list(filter(lambda a : isinstance(a, AbsoluteSectionAssertion), base_test.assertions))
  section_addresses = None
  if absolute_sections:
    absolute_sections_file, section_addresses = place_all_absolute_sections(absolute_sections)
    base_cocas_input.append(absolute_sections_file)

  
  for opt_level in ['0', '1', '2', '3', 's']:
    binary = None
    compiled = None
    try:
      compiled = list(map(lambda f : clang_compile(f, clang_path, include_paths, opt_level), c_files))
  
      binary = cocas_assemble_and_link(base_cocas_input + compiled)
      run_binary(ws, binary)
      check_assertions(ws, base_test, section_addresses, processor_info)
  
    except (CompilationError, CocasError, AssertionsError) as e:
      print_error_big(f'Error in end-to-end test {base_test.name} with optimization level -O{opt_level}:\n{e.message}', file = errors_stream)
      yield False
    else:
      yield True
    finally:
      if not binary is None:
        os.remove(str(binary))
      if not compiled is None:
        for file in compiled:
          os.remove(str(file))

  if not absolute_sections_file is None:
    os.remove(str(absolute_sections_file))

@tests_producer
def run_clang_driver_tests(ws, base_test, processor_info, clang_path, include_paths, errors_stream):
  for file in filter(lambda f: f.suffix == '.c', base_test.files):
    for opt_level in ['0', '1', '2', '3', 's']:
      obj_from_clang = None
      asm = None
      obj_from_cocas = None
      try:
        obj_from_clang = clang_compile_and_assemble(file, clang_path, include_paths, opt_level)
        asm = clang_compile(file, clang_path, include_paths, opt_level)
        obj_from_cocas = cocas_assemble(asm)
      except (CompilationError, CocasError) as e:
        print_error_big(f'Error in clang driver test {base_test.name} with optimization level -O{opt_level}:\n{e.message}', file = errors_stream)
        yield False
      else:
        if not compare_objs(obj_from_cocas, obj_from_clang):
          from_cocas, from_clang = get_objs_diff(obj_from_cocas, obj_from_clang)
          print_error_big(f'Error in clang driver test {base_test.name} with optimization level -O{opt_level}:\nClang driver and clang+cocas generated different output\nCocas:\n{from_cocas}\nClang:\n{from_clang}', file = errors_stream)
          yield False
        else:
          yield True
      finally:
        if not obj_from_clang is None:
          os.remove(str(obj_from_clang))
        if not asm is None:
          os.remove(str(asm))
        if not obj_from_cocas is None:
          os.remove(str(obj_from_cocas))
