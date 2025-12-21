from collections import namedtuple

BaseTest = namedtuple('Test', ['name', 'files', 'assertions'])
BaseTest.__str__ = lambda self : f'\033[1m{self.name}:\033[0m\n\tFiles:\n\t\t{'\n\t\t'.join(map(str, self.files))}\n\tAssertions:\n\t\t{'\n\t\t'.join(map(str, self.assertions))}'

RegisterAssertion = namedtuple('RegisterAssertion', ['reg', 'value'])
AbsoluteSectionAssertion = namedtuple('AbsoluteSectionAssertion', ['symbol', 'content'])
MemoryAssertion = namedtuple('MemoryAssertion', ['address', 'content'])

ProcessorInfo = namedtuple('ProcessorInfo', ['regs', 'regsizes', 'memsize'])
ProcessorInfo.__str__ = lambda self : f'\033[1mProcessor:\033[0m\n\tRegs: {' '.join(self.regs)}\n\tRegsizes: {' '.join(map(str, self.regsizes))}\n\tMemsize: {self.memsize}'
