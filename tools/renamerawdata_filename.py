import iricdevtool as idev
import subprocess
import os

def renamerawdata(path):
  """rename files with "rawdata" to "geodata"
  """

  if 'moc_' in path: return
  if 'qrc_' in path: return

  oldpath = path
  newpath = oldpath.replace('iricRd', 'iricGd')
  newpath = newpath.replace('rawdata', 'geodata')
  newpath = newpath.replace('rd_', 'gd_')

  os.rename(oldpath, newpath)
 
print('adding empty lines before visibility identifier...')
idev.recursiveExec("../libs", "\.(h|cpp|ui|pro|ts|qm|qrc|png|jpg)$", renamerawdata)

