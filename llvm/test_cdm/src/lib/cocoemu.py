from websockets.sync.client import connect
import json
import time
from .structs import ProcessorInfo
from .exceptions import CocoemuServerError

def connect_to_server(port):
  MAX_TRIES = 10

  tries = 0
  while tries < MAX_TRIES:
    try:
     return connect(f"ws://localhost:{port}")
    except ConnectionRefusedError:
      time.sleep(0.5)
    tries += 1

  raise CocoemuServerError(f"Failed to connect to server after {MAX_TRIES} attempts")

def check_server_response(ws):
  resp = ws.recv()
  resp_dict = json.loads(resp)
  if not resp_dict["status"] == "OK":
    raise CocoemuServerError(resp)
  return resp_dict

def init_server(ws):
  message = {
    "action": "init",
    "target": "cdm16",
    "memoryConfiguration": "vonNeumann",
  }
  ws.send(json.dumps(message))
  resp = check_server_response(ws)

  return ProcessorInfo(resp['registerNames'], resp['registerSizes'], resp['ramSize'])

def reset_server(ws):
  message = {
    "action": "reset",
  }
  ws.send(json.dumps(message))
  check_server_response(ws)

def load_image_to_server(ws, filepath):
  message = {
    "action": "load",
    "source": "path",
    "path": str(filepath),
  }
  ws.send(json.dumps(message))
  check_server_response(ws)

def run_server(ws):
  message = {
    "action": "run",
    "stopConditions": [],
  }
  ws.send(json.dumps(message))
  check_server_response(ws) # Run confirming
  check_server_response(ws) # Stop message

def get_regs_from_server(ws):
  message = {
    "action": "getRegisters",
  }
  ws.send(json.dumps(message))
  resp = check_server_response(ws)

  return resp['registers']

def get_memory_from_server(ws):
  message = {
    "action": "getMemory",
  }
  ws.send(json.dumps(message))
  resp = check_server_response(ws)

  return resp['bytes']

def run_binary(ws, binary):
  reset_server(ws)
  load_image_to_server(ws, binary)
  run_server(ws)
