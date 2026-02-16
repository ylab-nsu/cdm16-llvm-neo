from pathlib import Path
from dataclasses import dataclass

@dataclass
class RegisterAssertion:
    reg: str
    value: int

@dataclass
class AbsoluteSectionAssertion:
    symbol: str
    content: list[int]

@dataclass
class MemoryAssertion:
    address: int
    content: list[int]

@dataclass
class BaseTest:
    name: str
    files: list[Path]
    assertions: list[RegisterAssertion | AbsoluteSectionAssertion | MemoryAssertion]

    def __str__(self):
        files_string = '\n\t\t'.join(map(str, self.files))
        assertions_string = '\n\t\t'.join(map(str, self.assertions))
        return f'\033[1m{self.name}:\033[0m\n\tFiles:\n\t\t{files_string}\n\tAssertions:\n\t\t{assertions_string}'

@dataclass
class ProcessorInfo:
    regs: list[str]
    regsizes: list[int]
    memsize: int

    def __str__(self):
        regs_string = ' '.join(self.regs)
        regsizes_string = ' '.join(map(str, self.regsizes))
        return f"\033[1mProcessor:\033[0m\n\tRegs: {regs_string}\n\tRegsizes: {regsizes_string}\n\tMemsize: {self.memsize}"
