import iricdevtool as idev
import sys
import os
import shutil
import configparser
import functools
from pathlib import Path

configFile = os.path.join(sys.path[0], "./data/dirExt.prop")

debugDir = os.path.join(sys.path[0], "../libdlls/debug")
releaseDir = os.path.join(sys.path[0], "../libdlls/release")

def copydlls(file, dir):
  shutil.copy(file, dir)
  print('Copied dll: ' + file)

# read config
config = configparser.ConfigParser()
config.read(configFile)

# check config.
sections = ["release", "debug"]
# options  = ["cgnslib", "expat", "gdal", "geos", "hdf5", "iriclib", "netcdf", "openssl", "proj", "qwt", "shapelib", "szip", "udunits", "vtk", "yaml-cpp", "zlib"]
## options  = ["expat", "gdal", "geos", "hdf5", "iriclib", "libpng", "netcdf", "openssl", "poco", "proj", "qwt", "shapelib", "szip", "tiff", "udunits", "vtk", "yaml-cpp", "zlib"]
options  = ["expat", "gdal", "geos", "hdf5", "iriclib", "libpng", "netcdf", "openssl", "poco", "proj", "qwt", "shapelib", "sqlite3", "tiff", "udunits", "vtk", "yaml-cpp"]
for section in sections:
  if (section in config.sections()):
    for option in options:
      if (not (option in config.options(section))):
        sys.exit("Error: option \"{0:s}\" not specified".format(option))
  else:
    sys.exit("Error: section \"{0:s}\" not specified".format(section))

# directory for searching dll:
dirList = {}
for section in sections:
  dirList[section] = []
  for option in options:
    if ((option == "hdf5") or (option == "iriclib") or (option == "poco")  or (option == "qwt") ):
      continue
    else:
      dirList[section].append(config.get(section, option))

# dll to be excluded:
exList = ["msvcp120.dll", "msvcr120.dll"]

# dll files that are not included in dirList:
fileList = {}
fileList["release"] = [
  str(Path(config.get("release", "iriclib"), "iriclib.dll")),
  str(Path(config.get("release", "hdf5"),    "hdf5.dll")),
  str(Path(config.get("release", "hdf5"),    "hdf5_hl.dll")),
  str(Path(config.get("release", "hdf5"),    "szip.dll")),
  str(Path(config.get("release", "hdf5"),    "zlib.dll")),
  str(Path(config.get("release", "poco"),    "PocoFoundation.dll")),
  str(Path(config.get("release", "qwt"),     "qwt.dll"))
]
fileList["debug"] = [
  str(Path(config.get("debug",  "iriclib"),  "iriclibd.dll")),
  str(Path(config.get("debug",  "hdf5"),     "hdf5_D.dll")),
  str(Path(config.get("debug",  "hdf5"),     "hdf5_hl_D.dll")),
  str(Path(config.get("debug",  "hdf5"),     "szip_D.dll")),
  str(Path(config.get("debug",  "hdf5"),     "zlib_D.dll")),
  str(Path(config.get("debug",  "poco"),     "PocoFoundationd.dll")),
  str(Path(config.get("debug",  "qwt"),      "qwtd.dll")),
]

# copy dll
dstDir = {}
dstDir["debug"]   = debugDir
dstDir["release"] = releaseDir
for section in sections:
  for dir in dirList[section]:
    idev.dirFileExec(dir, "\.dll$", functools.partial(copydlls, dir=dstDir[section]), exList)
  for file in fileList[section]:
     copydlls(file, dstDir[section])
