from abc import ABC, abstractmethod
from dataclasses import dataclass
from test_cdm.testing_system.util.diff_printing import get_diff
from .processor import ProcessorState

class Assertion(ABC):
  @abstractmethod
  def check(self, processor_state: ProcessorState) -> None:
    pass

@dataclass
class RegisterAssertion(Assertion):
  reg: str
  value: int

  def check(self, processor_state: ProcessorState) -> None:
    if not processor_state.regs[self.reg] == self.value:
      raise AssertionError(f'Incorrect value of register {self.reg}. Expected: {self.value}, got: {processor_state.regs[self.reg]}')

@dataclass
class AbsoluteSectionAssertion(Assertion):
  symbol: str
  content: list[int]
  address: int | None = None

  def check(self, processor_state: ProcessorState) -> None:
    if isinstance(self.address, type(None)):
      return
    for index, value in enumerate(self.content):
      if not processor_state.mem[self.address + index] == value:
        raise AssertionError(f'Incorrect memory in section {self.symbol}:\n{get_diff(self.content, processor_state.mem[self.address : self.address + len(self.content)])}')

@dataclass
class MemoryAssertion(Assertion):
  address: int
  content: list[int]

  def check(self, processor_state: ProcessorState) -> None:
    for index, value in enumerate(self.content):
      if not processor_state.mem[self.address + index] == value:
        raise AssertionError(f'Incorrect memory content at {self.address}:\n{get_diff(self.content, processor_state.mem[self.address : self.address + len(self.content)])}')
