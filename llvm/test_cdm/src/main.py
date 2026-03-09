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
from producers.driver_only.driver_only_producer import DriverOnlyTestCaseProducer

if __name__ == "__main__":
  parser = argparse.ArgumentParser(__file__)
  parser.add_argument('-c', '--clang', type=Path, help='path to clang executable, default: ../build/bin/clang', required=False, default=Path(__file__).parent.parent.parent / 'build' / 'bin' / 'clang')
  parser.add_argument('-p', '--port', type=int, help='port for cocoemu-server binding, default: 7001', required=False, default=7001)
  parser.add_argument('-I', '--include', type=Path, action='append', dest='include_paths', help='add directory to headers search paths', default=[])
  parser.add_argument('-v', '--verbose', action='store_true')
  parser.add_argument('-l', '--log', action='store_true', help='output full test results, each on new line, instead of "." or "F"')
  parser.add_argument('tests_to_run', type=Path, nargs='+', help='tests to run')
  args = parser.parse_args()

  config = Configuration(args.port, args.verbose, args.log, args.tests_to_run, Path(shutil.which(args.clang)).resolve().absolute(), args.include_paths, Path(Path(__file__).parent.parent / 'resources').resolve().absolute())

  try:
    producers: dict[str, type[TestCaseProducer]] = {'end_to_end' : EndToEndTestCaseProducer, 'driver_only' : DriverOnlyTestCaseProducer}
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
