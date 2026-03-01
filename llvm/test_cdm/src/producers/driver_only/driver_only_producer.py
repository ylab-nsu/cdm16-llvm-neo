
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
