import iricdevtool as idev
import re as re
import argparse

parser = argparse.ArgumentParser(description='add override keywords to overriding functions')
parser.add_argument('fname', metavar='FUNC_NAMENAME', type=str, help='The name of functions to add override keywords') 

parser.add_argument('--dryrun', help='Only list up the files to be changes', action='store_true') 

args = parser.parse_args()

fname = args.fname
dryrun = args.dryrun

def addoverride(path):
  """ Add override to functions with name fname
  """

#  print("Checking: " + path)
  with open(path, 'r', encoding = 'utf-8') as f:
    lines = f.readlines()
    newlines = []
    for line in lines:
      newline = line
      if 'virtual ' in line:
        pass
      elif 'override' in line:
        pass
      else:
        m1 = re.search(r".+ (" + fname + ")\(.*\).*{$", line)
        m2 = re.search(r".+ (" + fname + ")\(.*\).*;$", line)
        if m1:
          newline = newline.replace(' {', ' override {')
        elif m2:
          newline = newline.replace(';', ' override;')

      newlines.append(newline)

  if lines == newlines:
    return

  if dryrun:
    print(path)
    return

  # write result
  with open(path, 'w', encoding = 'utf-8') as f:
    for line in newlines:
      line = line.replace('\n', '\r\n')
      f.write(line)

print('adding override to function ' + fname + ' ...')

idev.recursiveExec("../libs", "\.h$", addoverride)
idev.recursiveExec("../apps", "\.h$", addoverride)

