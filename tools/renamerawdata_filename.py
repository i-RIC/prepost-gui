import iricdevtool as idev
import subprocess
import os

def renamerawdata(path):
  """rename files with "rawdata" to "geodata"
  """

  oldpath = path
  newpath = oldpath.replace('iricRd', 'iricGd')
  newpath = newpath.replace('rawdata', 'geodata')
  newpath = newpath.replace('rd_', 'gd_')

  os.rename(oldpath, newpath)
 
print('adding empty lines before visibility identifier...')
idev.recursiveExec("../libs", "\.(h|cpp|pro|ts|qm|qrc|png|jpg)$", renamerawdata)

