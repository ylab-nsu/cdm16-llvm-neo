from dataclasses import dataclass

@dataclass
class TestParsingError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message
