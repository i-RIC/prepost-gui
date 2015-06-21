import iricdevtool as idev
import subprocess
import os

def renamerawdata(path):
  """rename files with "rawdata" to "geodata"
  """

  with open(path, 'r', encoding = 'utf-8') as f:
    lines = f.readlines()
    newlines = []
    for line in lines:
      newline = line
      newline = newline.replace("rawData", "geoData")
      newline = newline.replace("rawdata", "geodata")
      newline = newline.replace("RAWDATA", "GEODATA")
      newline = newline.replace("RawData", "GeoData")
      newline = newline.replace("rd_", "gd_")
      newline = newline.replace("RD_", "GD_")
      newline = newline.replace("Rd", "Gd")
      newline = newline.replace("WIZAGD", "WIZARD")
      newline = newline.replace("STANDAGD", "STANDARD")
      newline = newline.replace("cg_coogd_", "cg_coord_")
      newline = newline.replace(".readGeoData(", ".readRawData(")
      newline = newline.replace("cg_iRIC_GotoGeoDataTop", "cg_iRIC_GotoRawDataTop")
      newline = newline.replace("dgeo", "draw")
      newlines.append(newline)

  if lines == newlines:
    return

  # write result
  with open(path, 'w', encoding = 'utf-8', newline = '\r\n') as f:
    for line in newlines:
      f.write(line)
 
print('adding empty lines before visibility identifier...')
idev.recursiveExec("../libs", "\.(h|cpp|ui|pro)$", renamerawdata)
idev.recursiveExec("../apps", "\.(h|cpp|ui|pro)$", renamerawdata)

