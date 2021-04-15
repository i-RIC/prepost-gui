import iricdevtool as idev
import subprocess
import pathlib
import os

def lrelease(pro_filename):
  cwd = os.getcwd()
  proPath = pathlib.Path(pro_filename)
  parentPath = proPath.parent
  os.chdir(str(parentPath))
  try:
    subprocess.check_output('lrelease ' + proPath.name)
    print('lrelease executed for ' + proPath.name)
  except subprocess.CalledProcessError:
    pass
  os.chdir(cwd)

print('executing lrelease ...')
idev.recursiveExec("..", "\.pro$", lrelease)
