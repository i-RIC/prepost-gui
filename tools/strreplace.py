import iricdevtool as idev

def strreplace(path):
  """ Replace string
  """
  with open(path, 'r', encoding = 'utf-8') as f:
    lines = f.readlines()
    newlines = []
    for line in lines:
      newline = line.replace('containedFiles()', 'containedFiles() const')
      newlines.append(newline)

  if lines == newlines:
    return

  # write result
  with open(path, 'w', encoding = 'utf-8') as f:
    for line in newlines:
      line = line.replace('\n', '\r\n')
      f.write(line)

print('replacing strings...')

idev.recursiveExec("../libs", "\.cpp$", strreplace)
idev.recursiveExec("../apps", "\.cpp$", strreplace)

