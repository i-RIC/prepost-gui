import iricdevtool as idev
import re as re
import argparse

parser = argparse.ArgumentParser(description='rename functions')
parser.add_argument('name_from', metavar='NAME_FROM', type=str, help='The name from which to rename') 
parser.add_argument('name_to', metavar='NAME_TO', type=str, help='The name renamed to which') 
args = parser.parse_args()

name_from = args.name_from
name_to = args.name_to

def renamefunc(path):
  """ Rename function
  """
  with open(path, 'r', encoding = 'utf-8') as f:
    lines = f.readlines()
    newlines = []
    for line in lines:
      newline = line.replace(name_from, name_to)
      newlines.append(newline)

  if lines == newlines:
    return

  # write result
  with open(path, 'w', encoding = 'utf-8') as f:
    for line in newlines:
      line = line.replace('\n', '\r\n')
      f.write(line)

print('renaming function from ' + name_from + ' to ' + name_to + ' ...')

idev.recursiveExec("../libs", "\.(h|cpp)$", renamefunc)
idev.recursiveExec("../apps", "\.(h|cpp)$", renamefunc)

