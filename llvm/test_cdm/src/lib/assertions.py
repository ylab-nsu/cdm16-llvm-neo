from .exceptions import AssertionsError
from .cocoemu import get_regs_from_server
from .cocoemu import get_memory_from_server
from .util.diff_printing import get_diff
from .structs import RegisterAssertion
from .structs import MemoryAssertion
from .structs import AbsoluteSectionAssertion

def check_reg_assertion(ws, assertion, processor_info, regs):
  if not regs[processor_info.regs.index(assertion.reg)] == assertion.value:
      raise AssertionsError(f'Incorrect value of register {assertion.reg}. Expected: {assertion.value}, got: {regs[processor_info.regs.index(assertion.reg)]}')
  
def check_mem_assertion(ws, assertion, memory):
  for index, value in enumerate(assertion.content):
    if not memory[assertion.address + index] == value:
      raise AssertionsError(f'Incorrect memory content at {assertion.address}:\n{get_diff(assertion.content, memory[assertion.address : assertion.address + len(assertion.content)])}')

def check_sym_assertion(ws, assertion, section_addresses, memory):
  sec_addr = section_addresses[assertion.symbol]
  for index, value in enumerate(assertion.content):
    if not memory[sec_addr + index] == value:
      raise AssertionsError(f'Incorrect memory in section {assertion.symbol}:\n{get_diff(assertion.content, memory[sec_addr : sec_addr + len(assertion.content)])}')

def check_assertions(ws, base_test, section_addresses, processor_info):
  regs = get_regs_from_server(ws)
  memory = get_memory_from_server(ws)

  for ass in base_test.assertions:
    if isinstance(ass, RegisterAssertion):
      check_reg_assertion(ws, ass, processor_info, regs)
    elif isinstance(ass, MemoryAssertion):
      check_mem_assertion(ws, ass, memory)
    elif isinstance(ass, AbsoluteSectionAssertion):
      check_sym_assertion(ws, ass, section_addresses, memory)
