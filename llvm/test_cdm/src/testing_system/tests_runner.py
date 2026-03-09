import subprocess
import signal
import io
import sys
from pathlib import Path
from abc import ABC, abstractmethod
from collections.abc import Generator
from dataclasses import dataclass
from testing_system.test_case_producer import TestCaseProducer
from .cocoemu import CocoemuConnection
from .test_case import TestCase
from .parse.tests_parser import parse_all_tests
from .configuration import Configuration

def run_testing_system(config: Configuration, producers: dict[str, type[TestCaseProducer]]) -> int:
  server_proc = subprocess.Popen([
                                   "bin/cocoemu-server",
                                   "-p",
                                   str(config.cocoemu_port)
                                 ],
                                 cwd = sys.prefix,
                                 stdout = subprocess.DEVNULL)
  try:
    with CocoemuConnection(config.cocoemu_port) as connection:
      print("\033[32mServer initialized\033[0m")
      if config.verbose:
        print(connection.processor_info, end = "\n\n")

      producer_instances = dict(map(lambda item: (item[0], item[1](config, connection.processor_info)), producers.items()))

      test_cases = parse_all_tests(config.tests_to_run, producer_instances)
      if (config.verbose):
        print("\033[32mFound tests:\033[0m")
        print('\n\n'.join(map(str, test_cases)), end = "\n\n")
      else:
        print(f"\033[32mFound {len(test_cases)} tests\033[0m")

      succ, fails = run_tests(connection, config, test_cases)

      return -1 if fails else 0
  finally:
    server_proc.send_signal(signal.SIGINT)
    server_proc.wait()

def run_tests(connection: CocoemuConnection, config: Configuration, test_cases: list[TestCase]) -> tuple[int, int]:
  total_succ = 0
  total_fails = 0

  with io.StringIO() as errors_stream:
    try:
      for case in test_cases:
        result = case.run(connection, errors_stream)
        if result:
          total_succ+=1
          if config.log:
            print(f'[\033[32mSuccess\033[0m] {case.name}')
          else:
            print('\033[32m.\033[0m', end="", flush=True)
        else:
          total_fails+=1
          if config.log:
            print(errors_stream.getvalue(), end="")
            errors_stream.truncate(0)
          else:
            print('\033[31mF\033[0m', end="", flush=True)
    finally:
      print("\n", errors_stream.getvalue(), sep = "")
      print(f'Passed {total_succ} out of {total_succ + total_fails}')
  return (total_succ, total_fails)
