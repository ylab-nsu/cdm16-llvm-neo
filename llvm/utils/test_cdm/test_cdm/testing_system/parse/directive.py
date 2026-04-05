from dataclasses import dataclass

@dataclass
class Directive:
  name: str
  head: list[str]
  body: list[str]
  line: int
