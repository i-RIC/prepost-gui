import iricdevtool as idev
import subprocess
import os

def qmake(path):
  cwd = os.getcwd()
  os.chdir(path)
  try:
    subprocess.check_output('qmake -project')
    print('qmake executed for ' + path)
  except subprocess.CalledProcessError:
    pass
  os.chdir(cwd)

print('executing qmake ...')
idev.recursiveExecFolder("..", qmake)
