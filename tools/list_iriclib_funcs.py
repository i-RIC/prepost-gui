import subprocess
import os
import re

def get_dumpbin():
  cmd = 'dumpbin /all ..\\iriclib\\release\\iriclib.lib'
  p = subprocess.Popen(cmd, shell="True", stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  (stdout_data, stderr_data) = p.communicate()
  return stdout_data.decode('utf-8')

def filter_funcs(txt):
  lines = txt.split('\n')
  p = re.compile(" ([A-Z0-9_]+_F)")
  p2 = re.compile("_MUL_F")

  fnames = []
  for line in lines:
    match = p.search(line)
    if match is None: continue

    fname = match.group(1)
    
    if not p2.search(fname) is None: continue

    if fname not in fnames:
      fnames.append(fname)
  return fnames


dumpbin_txt = get_dumpbin()
funcs = filter_funcs(dumpbin_txt)

print('\n'.join(funcs))
