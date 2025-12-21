from pathlib import Path

def compare_objs(file1, file2):
  with Path.open(file1) as file1, Path.open(file2) as file2:
      for line1, line2 in zip(file1, file2):
        if not (line1 == line2):
          return False

  return True

def get_objs_diff(file1, file2):
  out1 = ""
  out2 = ""
  with Path.open(file1) as file1, Path.open(file2) as file2:
      for line1, line2 in zip(file1, file2):
        if not (line1 == line2):
          out1 += "\033[31m" + line1 + "\033[0m"
          out2 += "\033[31m" + line2 + "\033[0m"
        else:
          out1 += "\033[32m" + line1 + "\033[0m"
          out2 += "\033[32m" + line2 + "\033[0m"

  return (out1, out2)
