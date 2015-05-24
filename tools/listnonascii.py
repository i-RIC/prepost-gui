import iricdevtool as idev

def addconst(path):
  """ Add const keywords to function that should be made constant
  """
  print("Checking: " + path)
  try:
    with open(path, 'r', encoding = 'utf-8') as f:
      lines = f.readlines()

  except:
    print(path)

print('listing files with non-ascii characters...')

#listnonascii("japanese.txt")

idev.recursiveExec("../libs", "\.h$", addconst)
idev.recursiveExec("../apps", "\.h$", addconst)

