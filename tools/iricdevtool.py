from pathlib import Path
import re

def recursiveExec(basepath, pattern, func):
  p = Path(basepath)
  for subp in p.iterdir():
    subpath = str(subp)
    if (subp.name == '.svn'):
      continue
    if (subp.is_dir()):
      recursiveExec(subp, pattern, func)
    else:
      if (re.search(pattern, subpath)):
        func(subpath)

def dirFileExec(basepath, pattern, func, exList=[]):
  p = Path(basepath)
  for subp in p.iterdir():
    subpath = str(subp)
    if (subp.name == '.svn'):
      continue
    if (subp.is_dir()):
      # do nothing for subdirectories.
      continue
    if (subp.name in exList):
      # igonore files in exList
      continue
    else:
      if (re.search(pattern, subpath)):
        func(subpath)

def recursiveExecFolder(basepath, func):
  omitDirs = ['.svn', 'debug', 'release']
  p = Path(basepath)
  for subp in [x for x in p.iterdir() if x.is_dir()]:
    if (subp.name in omitDirs):
      continue
    recursiveExecFolder(str(subp), func)

  func(basepath)

def getTarget():
  tf = Path(__file__).with_name('target.txt')
  with open(str(tf), 'r') as f:
    return f.readline().strip()
