import subprocess
import json
import time
import sys
from pathlib import Path
from types import TracebackType
from dataclasses import dataclass
from websockets.sync.client import connect
from websockets.sync.client import ClientConnection
from typing import Any, cast
from .processor import ProcessorInfo, ProcessorState
from .configuration import Configuration

@dataclass
class CocoemuServerError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

class CocoemuConnection:
  processor_info: ProcessorInfo
  ws: ClientConnection
  server_proc: subprocess.Popen

  def __init__(self, config: Configuration) -> None:
    self.server_proc = subprocess.Popen([
                                     str(config.cocoemu_path),
                                     "-p",
                                     str(config.cocoemu_port)
                                   ],
                                   stdout = subprocess.DEVNULL)
    MAX_TRIES = 10

    tries = 0
    while tries < MAX_TRIES:
      tries += 1

      try:
       self.ws = connect(f"ws://localhost:{config.cocoemu_port}")
       break
      except ConnectionRefusedError:
        # Don't wait if it was last attempt
        if tries != MAX_TRIES:
          print("Failed to connect to server, retry...", file=sys.stderr)
          time.sleep(0.5)
    else:
      raise CocoemuServerError(f"Failed to connect to server after {MAX_TRIES} attempts")

    message = {
      "action": "init",
      "target": "cdm16",
      "memoryConfiguration": "vonNeumann",
    }
    self.ws.send(json.dumps(message))
    resp = self.check_server_response()
    self.processor_info = ProcessorInfo(resp['registerNames'], resp['registerSizes'], resp['ramSize'])

  def close(self) -> None:
    self.ws.close()
    self.server_proc.terminate()
    self.server_proc.wait()

  def __enter__(self) -> 'CocoemuConnection':
    return self

  def __exit__(self, type: type[BaseException], value: BaseException | None, traceback: TracebackType | None) -> None:
    self.close()

  def check_server_response(self) -> Any:
    resp = self.ws.recv()
    resp_dict = json.loads(resp)
    try:
      if not resp_dict["status"] == "OK":
        raise CocoemuServerError(str(resp))
    except KeyError:
      raise CocoemuServerError(f'No "status" field in cocoemu answer: {resp_dict}')
    return resp_dict

  def reset_server(self) -> None:
    message = {
      "action": "reset",
    }
    self.ws.send(json.dumps(message))
    self.check_server_response()

  def load_image_to_server(self, filepath: Path) -> None:
    message = {
      "action": "load",
      "source": "path",
      "path": str(filepath),
    }
    self.ws.send(json.dumps(message))
    self.check_server_response()

  def run_server(self) -> None:
    message = {
      "action": "run",
      "stopConditions": [],
    }
    self.ws.send(json.dumps(message))
    self.check_server_response() # Run confirmation
    self.check_server_response() # Stop message

  def get_regs_from_server(self) -> dict[str, int]:
    message = {
      "action": "getRegisters",
    }
    self.ws.send(json.dumps(message))
    resp = self.check_server_response()

    try:
      return dict(zip(self.processor_info.regs, resp['registers']))
    except KeyError:
      raise CocoemuServerError(f'No "registers" field in cocoemu answer: {resp}')

  def get_memory_from_server(self) -> list[int]:
    message = {
      "action": "getMemory",
    }
    self.ws.send(json.dumps(message))
    resp = self.check_server_response()

    try:
      return cast(list[int], resp['bytes'])
    except KeyError:
      raise CocoemuServerError(f'No "bytes" field in cocoemu answer: {resp}')

  def get_processor_state(self) -> ProcessorState:
    return ProcessorState(self.get_regs_from_server(), self.get_memory_from_server())

  def run_binary(self, binary: Path) -> None:
    self.reset_server()
    self.load_image_to_server(binary)
    self.run_server()
