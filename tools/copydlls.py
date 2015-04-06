import iricdevtool as idev
import shutil

debugDir = "../libdlls/debug"
releaseDir = "../libdlls/release"

def copydlls(path):
  if ("libdlls" in path):
    return
  if ("\\plugins\\" in path):
    return

  if ("\\debug\\" in path):
    shutil.copy(path, debugDir)
    print('Copied dll: ' + path)

  elif ("\\release\\" in path):
    shutil.copy(path, releaseDir)
    print('Copied dll: ' + path)

print('copying dlls...')
idev.recursiveExec("..", "\.dll$", copydlls)
