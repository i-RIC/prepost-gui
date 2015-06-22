import iricdevtool as idev
import shutil
import os;

targetDir = "../apps/iricgui/debug/languages"

def copydics(path):
  if "debug" in path:
    return

  shutil.copy(path, targetDir)
  print('Copied dic: ' + path)

print('copying dics...')
idev.recursiveExec("..", "\.qm$", copydics)
