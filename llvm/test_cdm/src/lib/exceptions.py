
class TestParsingError(Exception):
  def __init__(self, message = ""):
    self.message = message

class CocoemuServerError(Exception):
  def __init__(self, message = ""):
    self.message = message

class CompilationError(Exception):
  def __init__(self, message = ""):
    self.message = message

class CocasError(Exception):
  def __init__(self, message = ""):
    self.message = message

class AssertionsError(Exception):
  def __init__(self, message = ""):
    self.message = message
