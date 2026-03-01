from pathlib import Path

def compare_objs(file1: Path, file2: Path) -> bool:
  with Path.open(file1) as file1_content, Path.open(file2) as file2_content:
      for line1, line2 in zip(file1_content, file2_content):
        if not (line1 == line2):
          return False

  return True

def get_objs_diff(file1: Path, file2: Path) -> tuple[str, str]:
  out1 = ""
  out2 = ""
  with Path.open(file1) as file1_content, Path.open(file2) as file2_content:
      for line1, line2 in zip(file1_content, file2_content):
        if not (line1 == line2):
          out1 += "\033[31m" + line1 + "\033[0m"
          out2 += "\033[31m" + line2 + "\033[0m"
        else:
          out1 += "\033[32m" + line1 + "\033[0m"
          out2 += "\033[32m" + line2 + "\033[0m"

  return (out1, out2)
