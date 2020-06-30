import iricdevtool as idev
import subprocess
import os
from pathlib import Path
import re

def add_plugins(ret):
  p = Path('..\plugins')
  for subp in p.iterdir():
    if not subp.is_dir(): continue

    for subsubp in subp.iterdir():
      if not subsubp.is_dir(): continue

      name = subp.name + '_' + subsubp.name
      dllname = '${PREPOST_GUI}/plugins/' + subp.name + '/' + subsubp.name + '/release/' + subsubp.name + '.dll'
      lang_path = Path(str(subsubp) + '/languages')
      dic_names = list()
      for f in lang_path.iterdir():
        if '.qm' in f.name:
          dic_names.append(f.name)

      ret.append('# ' + name)
      ret.append('set(' + name)
      ret.append('  ' + dllname)
      for dic_name in dic_names:
        ret.append('  ${PREPOST_GUI}/plugins/' + subp.name + '/' + subsubp.name + '/languages/' + dic_name)
      ret.append(')')

      ret.append('# install(FILES ${' + name + '}')
      ret.append('#   DESTINATION ${CMAKE_INSTALL_PREFIX}/${SRC_FOLDER}/packages/' + subp.name + '_plugin.' + subsubp.name)
      ret.append('# )')

      ret.append('')

def make_dll_list_rec(path, list):
  for f in path.iterdir():
    if f.is_dir():
      make_dll_list_rec(f, list)
    elif re.search(r"release.+\.dll", str(f)):
      name = str(f)
      name = name.replace('\\', '/')
      name = name.replace('../libs', '${PREPOST_GUI}/libs')

      list.append(name)

def make_dll_list():
  ret = list()
  p = Path('../libs')
  make_dll_list_rec(p, ret)
  return ret

def make_dic_list_rec(path, list):
  for f in path.iterdir():
    if f.is_dir():
      make_dic_list_rec(f, list)
    elif re.search(r"\.qm$", str(f)):
      name = str(f)
      name = name.replace('\\', '/')
      name = name.replace('../libs', '${PREPOST_GUI}/libs')

      list.append(name)

def make_dic_list():
  ret = list()
  p = Path('../libs')
  make_dic_list_rec(p, ret)
  return ret

ret = list()
ret.append('#')
ret.append('# usage:')
ret.append('#   mkdir _build')
ret.append('#   cd _build')
ret.append('#   cmake -DCMAKE_INSTALL_PREFIX:PATH=C:/online_update -DPREPOST_GUI:PATH=C:/prepost-gui ..')
ret.append('')
ret.append('cmake_minimum_required(VERSION 3.1)')
ret.append('')
ret.append('if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)')
ret.append('  set(CMAKE_INSTALL_PREFIX c:/online_update CACHE PATH "Top level directory of online_update workspace" FORCE)')
ret.append('endif()')
ret.append('')
ret.append('if(NOT PREPOST_GUI)')
ret.append('  set(PREPOST_GUI c:/prepost-gui)')
ret.append('endif()')
ret.append('')

add_plugins(ret)

ret.append('# IRIC_ICO')
ret.append('set(IRIC_ICO')
ret.append('  ${PREPOST_GUI}/apps/iricgui/images/iconiRICFile.ico')
ret.append(')')
ret.append('')

ret.append('# IRIC_EXE')
ret.append('set(IRIC_EXE')
ret.append('  ${PREPOST_GUI}/apps/iricgui/release/iRIC.exe')
ret.append(')')
ret.append('')

dlls = make_dll_list()
ret.append('# IRIC_DLLS')
ret.append('set(IRIC_DLLS')
for dll in dlls:
  ret.append('  ' + dll)
ret.append(')')
ret.append('')

dics = make_dic_list()
ret.append('# IRIC_DICS')
ret.append('set(IRIC_DICS')
for dic in dics:
  ret.append('  ' + dic)
ret.append(')')
ret.append('')

ret.append('# IRIC_PY')
ret.append('set(IRIC_PY')
ret.append('  ${PREPOST_GUI}/apps/iricpython/iric.py')
ret.append(')')
ret.append('')

ret.append('# Install IRIC')
ret.append('install(FILES ${IRIC_ICO} ${IRIC_EXE} ${IRIC_DLLS}')
ret.append('  DESTINATION ${CMAKE_INSTALL_PREFIX}/${SRC_FOLDER}/packages/gui.prepost/data')
ret.append(')')

ret.append('install(FILES ${IRIC_DICS}')
ret.append('  DESTINATION ${CMAKE_INSTALL_PREFIX}/${SRC_FOLDER}/packages/gui.prepost/data/languages')
ret.append(')')

ret.append('install(FILES ${IRIC_PY}')
ret.append('  DESTINATION ${CMAKE_INSTALL_PREFIX}/${SRC_FOLDER}/packages/gui.prepost/data/pythonmodule')
ret.append(')')
ret.append('')

with open('../dist/CMakeLists.txt', 'w', encoding='utf-8') as f:
  for l in ret:
    f.write(l + '\n')

