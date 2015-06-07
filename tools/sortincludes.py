import iricdevtool as idev
import subprocess
import os

def sortincludes(path):
  if "moc_" in path: return

  with open(path, 'r', encoding = 'shift_jis') as f:
    lines = f.readlines()

  ui_incs = []
  local_incs = []
  vtk_incs = []
  qt_incs = []
  iric_incs = []
  std_incs = []

  while len(lines) > 0:
    line = lines.pop(0)
    line = line.strip()
    if not line: continue
    if not "#include" in line:
      line = line + '\r\n'
      lines.insert(0, line)
      break

    if '"ui_' in line:
      ui_incs.append(line)
    elif '"' in line:
      local_incs.append(line)
    elif '<QVTK' in line or '<vtk' in line:
      vtk_incs.append(line)
    elif '<Q' in line:
      qt_incs.append(line)
    elif '/' in line:
      iric_incs.append(line)
    else:
      std_incs.append(line)

  ui_incs.sort()
  local_incs.sort()
  vtk_incs.sort()
  qt_incs.sort()
  iric_incs.sort()
  std_incs.sort()

  # write result
  with open(path, 'w', encoding = 'shift_jis') as f:
    if len(ui_incs) > 0:
      for line in ui_incs:
        f.write(line + '\r\n')
      f.write('\r\n')

    if len(local_incs) > 0:
      for line in local_incs:
        f.write(line + '\r\n')
      f.write('\r\n')

    if len(iric_incs) > 0:
      for line in iric_incs:
        f.write(line + '\r\n')
      f.write('\r\n')

    if len(qt_incs) > 0:
      for line in qt_incs:
        f.write(line + '\r\n')
      f.write('\r\n')

    if len(vtk_incs) > 0:
      for line in vtk_incs:
        f.write(line + '\r\n')
      f.write('\r\n')

    if len(std_incs) > 0:
      for line in std_incs:
        f.write(line + '\r\n')
      f.write('\r\n')

    for line in lines:
      f.write(line)


print('sorting includes in cpp files...')
idev.recursiveExec("../libs", "\.cpp$", sortincludes)
idev.recursiveExec("../apps", "\.cpp$", sortincludes)
