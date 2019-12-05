import iricdevtool as idev
import shutil
import os;

targetDir = "../languages"

def copydics(path):
  if "debug" in path:
    return
  if "..\\languages" in path:
    return
  if "solvers" in path:
    return

  print (path)
  shutil.copy(path, targetDir)
  print('Copied dic: ' + path)

print('copying dics...')
idev.recursiveExec("..", "\.ts$", copydics)
