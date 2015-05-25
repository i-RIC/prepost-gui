import iricdevtool as idev
import re as re

def addoverridelist(path):
  """ Add override to functions with name fname
  """

  if 'ui_' in path:
    return

  print("Fixing " + path + "...")
  with open(path, 'r', encoding = 'utf-8') as f:
    lines = f.readlines()
    newlines = []
    for line in lines:
      newline = line
      if '= 0' in line:
        pass
      elif 'override' in line:
        pass
      elif 'static ' in line:
        pass
      else:
        m1 = re.search(r".+ ([A-Za-z0-9_]+)\(.*\).*{", line)
        m2 = re.search(r".+ ([A-Za-z0-9_]+)\(.*\).*;$", line)
        if m1:
          newline = newline.replace(' {', ' override {')
        elif m2:
          newline = newline.replace(';', ' override;')
      newlines.append(newline)

  if lines == newlines:
    return

  # write result
  with open(path, 'w', encoding = 'utf-8') as f:
    for line in newlines:
      line = line.replace('\n', '\r\n')
      f.write(line)

print('adding override to functions ...')

idev.recursiveExec("../libs", "\.h$", addoverridelist)
idev.recursiveExec("../apps", "\.h$", addoverridelist)

