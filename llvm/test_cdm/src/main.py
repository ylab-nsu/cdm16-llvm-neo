import argparse
import sys
import shutil
from pathlib import Path
from testing_system.tests_runner import run_testing_system
from testing_system.cocoemu import CocoemuServerError
from testing_system.parse.tests_parser import TestParsingError
from testing_system.util.errors_printing import print_error
from testing_system.test_case_producer import TestCaseProducer, TestProducingError
from testing_system.configuration import Configuration
from producers.end_to_end.end_to_end_producer import EndToEndTestCaseProducer

if __name__ == "__main__":
  parser = argparse.ArgumentParser(__file__)
  parser.add_argument('-c', '--clang', type=Path, help='path to clang executable', required=False)
  parser.add_argument('-p', '--port', type=int, help='port for cocoemu-server binding', required=False, default=7001)
  parser.add_argument('-I', '--include', type=Path, action='append', dest='include_paths', help='add directory to headers search paths', default=[])
  parser.add_argument('-v', '--verbose', action='store_true')
  parser.add_argument('tests_dir', type=Path, help='path to tests directory')
  args = parser.parse_args()

  config = Configuration(args.port, args.verbose, args.tests_dir, Path(shutil.which(args.clang)).resolve().absolute(), args.include_paths, Path(Path(__file__).parent.parent / 'resources').resolve().absolute())

  try:
    producers: dict[str, TestCaseProducer] = {'end_to_end' : EndToEndTestCaseProducer(config)}
    sys.exit(run_testing_system(config, producers))
  except KeyboardInterrupt:
    print("")
  except CocoemuServerError as e:
    print_error(f"Cocoemu-server responded with error: {str(e)}", file = sys.stderr)
  except (TestParsingError, TestProducingError) as e:
    print_error(str(e))
  else:
    sys.exit(0)

  sys.exit(1)
