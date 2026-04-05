import argparse
import sys
import shutil
from pathlib import Path
from test_cdm.testing_system.tests_runner import run_testing_system
from test_cdm.testing_system.cocoemu import CocoemuServerError
from test_cdm.testing_system.parse.test_parsing_error import TestParsingError
from test_cdm.testing_system.util.errors_printing import print_error
from test_cdm.testing_system.test_case_producer import TestProducingError
from test_cdm.testing_system.configuration import Configuration, InvalidConfigurationError

def main(base_config: Configuration) -> int:

  parser = argparse.ArgumentParser(__file__)
  parser.add_argument('--cocas', type=Path, help=f'path to cocas executable, default: {base_config.cocas_path}', required=False)
  parser.add_argument('-p', '--port', type=int, help='port for cocoemu-server binding, default: 7001', required=False, default=7001)
  parser.add_argument('-I', '--include', type=Path, action='append', dest='include_paths', help='add directory to headers search paths', default=[])
  parser.add_argument('-v', '--verbose', action='store_true')
  parser.add_argument('searching_points', type=Path, nargs='+', help='directories/files from which to search for tests recursevely')
  args = parser.parse_args()

  if not args.cocas is None:
    resolved = shutil.which(args.cocas)
    if resolved is None:
      print_error(f'Cannot find {str(args.cocas)}', file = sys.stderr)
      return 1;
    base_config.cocas_path = Path(resolved)
  base_config.include_paths.extend(args.include_paths)

  base_config.cocoemu_port = args.port
  base_config.verbose = args.verbose
  base_config.log = not sys.stdout.isatty()
  base_config.search_points = args.searching_points

  try:
    base_config.verify()

    return run_testing_system(base_config)
  except KeyboardInterrupt:
    print("")
  except CocoemuServerError as e:
    print_error(f"Cocoemu-server responded with error: {str(e)}", file = sys.stderr)
  except (TestParsingError, TestProducingError, InvalidConfigurationError) as e:
    print_error(str(e))
  else:
    return 0

  return 1
