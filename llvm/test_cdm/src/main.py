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
  parser.add_argument('--cocas', type=Path, help='path to cocas executable, default: .venv/bin/cocas', required=False, default=Path(f'{sys.prefix}/bin/cocas'))
  parser.add_argument('-t', '--llvm-tools-dir', type=Path, help='path to directory with just built llvm tools. Required: clang, llvm-objcopy', required=False, default=Path(__file__).parent.parent.parent / 'build' / 'bin')
  parser.add_argument('-p', '--port', type=int, help='port for cocoemu-server binding, default: 7001', required=False, default=7001)
  parser.add_argument('-I', '--include', type=Path, action='append', dest='include_paths', help='add directory to headers search paths', default=[])
  parser.add_argument('-v', '--verbose', action='store_true')
  parser.add_argument('searching_points', type=Path, nargs='+', help='directories/files from which to search for tests recursevely')
  args = parser.parse_args()

  log = not sys.stdout.isatty()

  if not (args.llvm_tools_dir / 'clang').exists():
    print("No clang binary in llvm tools directory was found", file = sys.stderr)
    sys.exit(-1)
  if not (args.llvm_tools_dir / 'llvm-objcopy').exists():
    print("No llvm-objcopy in llvm tools directory was found", file = sys.stderr)
    sys.exit(-1)

  config = Configuration(
                         args.port,
                         args.verbose,
                         log,
                         args.searching_points,
                         (args.llvm_tools_dir / 'clang').resolve().absolute(),
                         (args.llvm_tools_dir / 'llvm-objcopy').resolve().absolute(),
                         Path(shutil.which(args.cocas)).resolve().absolute(),
                         args.include_paths,
                         Path(Path(__file__).parent.parent / 'resources').resolve().absolute()
                        )

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
