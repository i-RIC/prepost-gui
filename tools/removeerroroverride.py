import iricdevtool as idev
import re as re
import subprocess as sp
import os

def fixoverride(path):
  cwd = os.getcwd()
  os.chdir(path)

  if not os.path.isfile('Makefile'):
    os.chdir(cwd)
    return

  print('Fixing overrides in ' + path + '...')

  execMake()

  os.chdir(cwd)

def execMake():
  err_file = 'error.txt'

  while True:
    err = open(err_file, 'w', encoding='utf-8')
    ret = sp.call('make', stdout=sp.DEVNULL, stderr=err)
    # ret = sp.call('make', stderr=err)
    del err

    if ret == 0:
      break

    with open(err_file, 'r', encoding='utf-8') as f:
      lines = f.readlines()
      for l in lines:
        if 'error:' in l:
          m = re.search(r"(.+\.h)\:([0-9]+)\:", l)
          if not m:
            continue

          g = m.groups()
          filename = g[0]
          linenum = int(g[1])
          
          removeOverride(filename, linenum)

  os.unlink(err_file)

def removeOverride(fname, linenum):
  linenum = linenum - 1
  # read data
  with open(fname, 'r', encoding = 'utf-8') as f:
    lines = f.readlines()
    l = lines[linenum]
    l = l.replace(' override', '')
    lines[linenum] = l
    print('line ' + str(linenum) + ' in ' + fname + ' fixed')
    print(l, end="")

  # write result
  with open(fname, 'w', encoding = 'utf-8') as f:
    for l in lines:
      l = l.replace('\n', '\r\n')
      f.write(l)

  

print('fixing override in each folders ...')
idev.recursiveExecFolder("../libs", fixoverride)
idev.recursiveExecFolder("../apps", fixoverride)


