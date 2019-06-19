## prepost-gui [![Build status](https://ci.appveyor.com/api/projects/status/f5lfc0iym90xpb7b?svg=true)](https://ci.appveyor.com/project/i-RIC/prepost-gui)

The paths to install external libraries, such as VTK, Qwt, proj.4 differs on each development environment, so the 
paths are now excluded from src.pro, and stored in paths.pri.

This folder contains paths_std.pri, so copy it to paths.pri, and modify it to fit your environment. Or use the
paths.pri that's created when building https://github.com/i-RIC/iricdev (or by running create-files.cmd).

## Automatic upload to https://github.com/i-RIC/online_update.git

If the following conditions are all true then the output of this build is added to the dev_src directory of the
i-RIC/online_update repository:

1. Is from this repository (https://github.com/i-RIC/prepost-gui.git)
2. The commit is on the master branch
3. It is not a pull request
4. The Release Configuration is being built
5. The build is successful

## Steps to build on appveyor

1. Build iRIC GUI
   1. Load Qt environment
   2. Download lastest master branch of i-RIC/iricdev
   3. Download pre-built libraries from i-RIC/iricdev
   4. Run qmake to create Visual Studio solution and project files
   5. Run msbuild to build the Visual Studio solution

2. Clone the i-RIC/online_update repository

3. Update the following files to modify version number and release date
   - dev_src/packages/gui.prepost/data/definition.xml
   - dev_src/packages/gui.prepost/meta/package.xml
   1. This is done using powershell's builtin string, date, and regular expression tools

4. Copy the binaries to dev_src/packages/gui.prepost/data
   1. Run cmake to create an INSTALL.vcxproj project
   2. Run msbuild to distribute the prepost-gui files

5. Run repogen to build packages and Updates.xml for online update
   1. Run python script dev_src/build_update_repository.py

6. Commit and push results to i-RIC/online_update
   1. Create new user iric-appveyor
   2. Add user iric-appveyor as collaborator on i-RIC/online_update repository
   3. Add personal access token in Settings / Developer settings (in iric-appveyor account)
   4. Use appveyor website to encrypt personal access token from i-RIC account
