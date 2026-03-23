from dataclasses import dataclass

@dataclass
class Directive:
  name: str
  head: list[str]
  body: list[str]
  line: int

  def __str__(self) -> str:
    head_string = ', '.join(self.head)
    body_string = ', '.join(self.body)
    return f'{self.name}({head_string}) {body_string}'
