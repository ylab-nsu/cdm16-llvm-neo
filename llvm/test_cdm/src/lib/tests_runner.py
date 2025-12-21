import subprocess
import signal
import io
import sys
from .tests_parser import get_tests
from .cocoemu import connect_to_server
from .cocoemu import init_server

def run_tests_producers(cocoemu_port, tests_dir, clang_path, include_paths, producers):
  print(sys.prefix)
  server_proc = subprocess.Popen([
                                   "bin/cocoemu-server",
                                   "-p",
                                   str(cocoemu_port)
                                 ],
                                 cwd = sys.prefix,
                                 stdout = subprocess.DEVNULL)
  try:
    with connect_to_server(cocoemu_port) as ws:
      processor_info = init_server(ws)
      print("\033[32mServer initialized\033[0m")
      print(processor_info, end = "\n\n")

      tests = get_tests(tests_dir, processor_info)
      print("\033[32mFound tests:\033[0m")
      print('\n\n'.join(map(str, tests)), end = "\n\n")

      total_succ = 0
      total_fails = 0

      with io.StringIO() as errors_stream:
        try:
          for base_test in tests:
            for prod in producers:
              succ, fails = prod(ws, base_test, processor_info, clang_path, include_paths, errors_stream)
              total_fails += fails
              total_succ += succ
        finally: 
          print("\n", errors_stream.getvalue(), sep = "")

      print(f'Passed {total_succ} out of {total_succ + total_fails}')
      return (total_succ, total_fails)
  finally:
    server_proc.send_signal(signal.SIGINT)
