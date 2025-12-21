import re
from ..exceptions import TestParsingError

_DIRECTIVE_PATTERN = re.compile("^.*?//\\s*CHECK\\s(.*)\n")
_DIRECTIVE_INNER_PATTERN = re.compile("\\s*(\\S+)\((.*)\)\\s*(.*)")

def iter_directives(filepath):
  if not filepath.is_file():
    raise ValueError("Expected regular file")

  with filepath.open() as file:
    line_num = 1
  
    for line in file:
      m = _DIRECTIVE_PATTERN.fullmatch(line)
      if not m is None:
        m = _DIRECTIVE_INNER_PATTERN.fullmatch(m.group(1))
        if not m is None:
          yield (line_num, m.group(1), m.group(2), m.group(3))
        else:
          raise TestParsingError(f'{filepath.name}:{line_num} Wrong directive format')
      line_num += 1
