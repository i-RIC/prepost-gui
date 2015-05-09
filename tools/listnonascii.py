import iricdevtool as idev
import os

def listnonascii(path):
  """ List files that contains non_ascii characters.
  """
#  print("Checking: " + path)
  try:
    with open(path, 'r', encoding = 'utf-8') as f:
      lines = f.readlines()

  except:
    print(path)

print('listing files with non-ascii characters...')

#listnonascii("japanese.txt")

idev.recursiveExec("../libs", "\.(cpp|h)$", listnonascii)
idev.recursiveExec("../apps", "\.(cpp|h)$", listnonascii)
