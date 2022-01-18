:: Create symbolic links to installed iRIC_v4 solvers/plugins/gridcreators/udunits
::
mklink /d ..\apps\solvers                    %USERPROFILE%\iRIC_v4\solvers
mklink /d ..\apps\gridexporter_plugins       %USERPROFILE%\iRIC_v4\gridexporter_plugins
mklink /d ..\apps\gridimporter_plugins       %USERPROFILE%\iRIC_v4\gridimporter_plugins
@REM mklink /d ..\apps\gridcreators               %USERPROFILE%\iRIC_v4\gridcreators
@REM mklink /d ..\apps\Miniconda3                 %USERPROFILE%\iRIC_v4\Miniconda3
mklink /d ..\apps\iricgui\debug\udunits      %USERPROFILE%\iRIC_v4\guis\prepost\udunits
mklink /d ..\apps\iricgui\release\udunits    %USERPROFILE%\iRIC_v4\guis\prepost\udunits
