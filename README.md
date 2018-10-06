## prepost-gui [![Build status](https://ci.appveyor.com/api/projects/status/f5lfc0iym90xpb7b?svg=true)](https://ci.appveyor.com/project/i-RIC/prepost-gui)

The paths to install external libraries, such as VTK, Qwt, proj.4 differs on each development environment, so the 
paths are now excluded from src.pro, and stored in paths.pri.

This folder contains paths_std.pri, so copy it to paths.pri, and modify it to fit your environment. Or use the
paths.pri that's created when building https://github.com/i-RIC/iricdev (or by running create-files.cmd).
