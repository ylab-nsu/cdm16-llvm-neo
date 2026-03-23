from dataclasses import dataclass

@dataclass
class ProcessorInfo:
  regs: list[str]
  regsizes: list[int]
  memsize: int

  def __str__(self) -> str:
      regs_string = ' '.join(self.regs)
      regsizes_string = ' '.join(map(str, self.regsizes))
      return f"\033[1mProcessor:\033[0m\n\tRegs: {regs_string}\n\tRegsizes: {regsizes_string}\n\tMemsize: {self.memsize}"

@dataclass
class ProcessorState:
  regs: dict[str, int]
  mem: list[int]

