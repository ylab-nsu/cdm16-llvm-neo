
def print_error(err, **kvargs):
  print(f'[\033[31mError\033[0m] {err}', **kvargs)

def print_error_big(err, **kvargs):
  print(f"\033[1m=============================================\033[0m\n[\033[31mError\033[0m]\n{err}", **kvargs)
