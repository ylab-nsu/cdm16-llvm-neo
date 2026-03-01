import json
import time
from pathlib import Path
from dataclasses import dataclass
from websockets.sync.client import connect
from websockets.sync.client import ClientConnection
from typing import Any, cast
from .processor import ProcessorInfo, ProcessorState

@dataclass
class CocoemuServerError(Exception):
  message: str
  def __str__(self) -> str:
    return self.message

class CocoemuConnection:
  processor_info: ProcessorInfo
  ws: ClientConnection

  def __init__(self, port: int) -> None:
    MAX_TRIES = 10

    tries = 0
    while tries < MAX_TRIES:
      try:
       self.ws = connect(f"ws://localhost:{port}")
       break
      except ConnectionRefusedError:
        time.sleep(0.5)
      tries += 1
    else:
      raise CocoemuServerError(f"Failed to connect to server after {MAX_TRIES} attempts")

    self.ws.send(
            """
            {
              "action": "init",
              "target": "cdm16",
              "memoryConfiguration": "vonNeumann"
            }
            """
          )
    resp = self.check_server_response()
    self.processor_info = ProcessorInfo(resp['registerNames'], resp['registerSizes'], resp['ramSize'])

  def __enter__(self) -> CocoemuConnection:
    return self

  def __exit__(self, type, value, traceback) -> None:
    self.ws.close()

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
    self.ws.send(
            """
            {
              "action": "reset"
            }
            """
           )
    self.check_server_response()

  def load_image_to_server(self, filepath: Path) -> None:
    self.ws.send(
            f"""
             {{
               "action": "load",
               "source": "path",
               "path" = "{str(filepath)}"
             }}
             """
           )
    self.check_server_response()

  def run_server(self) -> None:
    self.ws.send(
            """
            {
              "action": "run",
              "stopConditions": []
            }
            """
           )
    self.check_server_response() # Run confirmation
    self.check_server_response() # Stop message

  def get_regs_from_server(self) -> dict[str, int]:
    self.ws.send(
            """
            {
              "action": "getRegisters"
            }
            """
           )
    resp = self.check_server_response()

    try:
      return dict(zip(self.processor_info.regs, resp['registers']))
    except KeyError:
      raise CocoemuServerError(f'No "registers" field in cocoemu answer: {resp}')

  def get_memory_from_server(self) -> list[int]:
    self.ws.send(
            """
            {
              "action": "getMemory"
            }
            """
           )
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
