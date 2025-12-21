import argparse
import sys
from lib.tests_runner import run_tests_producers
from lib.exceptions import CocoemuServerError
from lib.exceptions import TestParsingError
from lib.util.errors_printing import print_error
from tests_producers import *

if __name__ == "__main__":
  parser = argparse.ArgumentParser(__file__)
  parser.add_argument('-c', '--clang', type=Path, help='path to clang executable', required=True)
  parser.add_argument('-p', '--port', type=int, help='port for cocoemu-server binding', required=False, default=7001)
  parser.add_argument('-I', '--include', type=Path, action='append', dest='include_paths', help='add directory to headers search paths', default=[])
  parser.add_argument('tests_dir', type=Path, help='path to tests directory')
  args = parser.parse_args()

  try:
    # TODO: Add run_clang_driver_tests producer
    _, fails = run_tests_producers(args.port, args.tests_dir, args.clang, args.include_paths, [run_end_to_end_tests])
    sys.exit(1 if fails else 0)
  except KeyboardInterrupt:
    print("")
  except CocoemuServerError as e:
    print_error(f"Cocoemu-server responded with error: {e.message}", file = sys.stderr)
  except TestParsingError as e:
    print_error(e.message)
  else:
    sys.exit(0)

  sys.exit(1)
