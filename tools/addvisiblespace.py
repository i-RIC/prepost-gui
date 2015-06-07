import iricdevtool as idev
import subprocess
import os

def addvisiblespace(path):
  """Add one empty line after 'public:' etc.
  """
  if "ui_" in path: return

  special_lines = ['public:', 'protected:', 'private:', 
    'public slots:', 'protected slots:', 'private slots:', 'signals:']

  with open(path, 'r', encoding = 'shift_jis') as f:
    lines = f.readlines()

  newlines = []
  last_is_empty = False

  for line in lines:
    stripedline = line.strip()
    if stripedline in special_lines and (not last_is_empty):
      newlines.append('\r\n')

    line = line.replace('\n', '\r\n')
    newlines.append(line)

    last_is_empty = (stripedline == '')

  # write result
  with open(path, 'w', encoding = 'shift_jis') as f:
    for line in newlines:
      f.write(line)

print('adding empty lines before visibility identifier...')
idev.recursiveExec("../libs", "\.h$", addvisiblespace)
idev.recursiveExec("../apps", "\.h$", addvisiblespace)
