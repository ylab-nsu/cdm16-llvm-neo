from typing import Any

def get_diff(expected: list[Any], actual: list[Any], **kvargs: Any) -> str:
  out = "Expected:\n"
  for val1, val2 in zip(expected, actual):
    if val1 == val2:
      out += "\033[32m" + f"{val1:X}" + "\033[0m "
    else:
      out += "\033[31m" + f"{val1:X}" + "\033[0m "
  out += "\nGot:\n"
  for val1, val2 in zip(expected, actual):
    if val1 == val2:
      out += "\033[32m" + f"{val2:X}" + "\033[0m "
    else:
      out += "\033[31m" + f"{val2:X}" + "\033[0m "

  return out
