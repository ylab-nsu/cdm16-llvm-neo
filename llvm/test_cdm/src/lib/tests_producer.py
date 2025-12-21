
def tests_producer(func):
  def wrapper(ws, base_test, processor_info, clang_path, include_paths, errors_stream):
    succ, fails = 0, 0
    for res in func(ws, base_test, processor_info, clang_path, include_paths, errors_stream):
      if res:
        print("\033[32m.\033[0m", end = "", flush=True)
        succ+=1
      else:
        print("\033[31mF\033[0m", end = "", flush=True)
        fails+=1
    return (succ, fails)

  return wrapper
