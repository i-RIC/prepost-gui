#
# usage:
#   mkdir _build
#   cd _build
#   cmake -DCMAKE_INSTALL_PREFIX:PATH=C:/online_update -DPREPOST_GUI:PATH=C:/prepost-gui ..

cmake_minimum_required(VERSION 3.1)


if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX c:/online_update CACHE PATH "Top level directory of online_update workspace" FORCE)
endif()

if(NOT PREPOST_GUI)
  set(PREPOST_GUI c:/prepost-gui)
endif()

# GRID_EXPORTER.structured2dgridnayscsv
set(GRID_EXPORTER.structured2dgridnayscsv
  ${PREPOST_GUI}/plugins/gridexporter/structured2dgridnayscsvexporter/release/structured2dgridnayscsvexporter.dll
  ${PREPOST_GUI}/plugins/gridexporter/structured2dgridnayscsvexporter/languages/structured2dgridnayscsvexporter_ja_JP.qm
  )
#install(FILES ${GRID_EXPORTER.structured2dgridnayscsv}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gridexporter_plugin.structured2dgridnayscsv/data
#  )

# GRID_EXPORTER.structured2dgridnaysgrid
set(GRID_EXPORTER.structured2dgridnaysgrid
  ${PREPOST_GUI}/plugins/gridexporter/structured2dgridnaysgridexporter/release/structured2dgridnaysgridexporter.dll
  ${PREPOST_GUI}/plugins/gridexporter/structured2dgridnaysgridexporter/languages/structured2dgridnaysgridexporter_ja_JP.qm
  )
#install(FILES ${GRID_EXPORTER.structured2dgridnaysgrid}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gridexporter_plugin.structured2dgridnaysgrid/data
#  )

# GRID_EXPORTER.structured2dgridvtk
set(GRID_EXPORTER.structured2dgridvtk
  ${PREPOST_GUI}/plugins/gridexporter/structured2dgridvtkexporter/release/structured2dgridvtkexporter.dll
  ${PREPOST_GUI}/plugins/gridexporter/structured2dgridvtkexporter/languages/structured2dgridvtkexporter_ja_JP.qm
  )
#install(FILES ${GRID_EXPORTER.structured2dgridvtk}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gridexporter_plugin.structured2dgridvtk/data
#  )

# GRID_EXPORTER.unstructured2dgridvtk
set(GRID_EXPORTER.unstructured2dgridvtk
  ${PREPOST_GUI}/plugins/gridexporter/unstructured2dgridvtkexporter/release/unstructured2dgridvtkexporter.dll
  ${PREPOST_GUI}/plugins/gridexporter/unstructured2dgridvtkexporter/languages/unstructured2dgridvtkexporter_ja_JP.qm
  )
#install(FILES ${GRID_EXPORTER.unstructured2dgridvtk}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gridexporter_plugin.unstructured2dgridvtk/data
#  )


# GRID_IMPORTER.structured2dgridnayscsv
set(GRID_IMPORTER.structured2dgridnayscsv
  ${PREPOST_GUI}/plugins/gridimporter/structured2dgridnayscsvimporter/release/structured2dgridnayscsvimporter.dll
  ${PREPOST_GUI}/plugins/gridimporter/structured2dgridnayscsvimporter/languages/structured2dgridnayscsvimporter_ja_JP.qm
  )
#install(FILES ${GRID_IMPORTER.structured2dgridnayscsv}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gridimporter_plugin.structured2dgridnayscsv/data
#  )

# GRID_IMPORTER.structured2dgridnaysgrid
set(GRID_IMPORTER.structured2dgridnaysgrid
  ${PREPOST_GUI}/plugins/gridimporter/structured2dgridnaysgridimporter/release/structured2dgridnaysgridimporter.dll
  ${PREPOST_GUI}/plugins/gridimporter/structured2dgridnaysgridimporter/languages/structured2dgridnaysgridimporter_ja_JP.qm
  )
#install(FILES ${GRID_IMPORTER.structured2dgridnaysgrid}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gridimporter_plugin.structured2dgridnaysgrid/data
#  )

# GRID_IMPORTER.unstructured2dgridtriangle
set(GRID_IMPORTER.unstructured2dgridtriangle
  ${PREPOST_GUI}/plugins/gridimporter/unstructured2dgridtriangleimporter/release/unstructured2dgridtriangleimporter.dll
  ${PREPOST_GUI}/plugins/gridimporter/unstructured2dgridtriangleimporter/languages/unstructured2dgridtriangleimporter_ja_JP.qm
  )
#install(FILES ${GRID_IMPORTER.unstructured2dgridtriangle}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gridimporter_plugin.unstructured2dgridtriangle/data
#  )


# IRIC_ICO
set(IRIC_ICO
  ${PREPOST_GUI}/apps/iricgui/images/iconiRICFile.ico
  )

# IRIC_EXE
set(IRIC_EXE
  ${PREPOST_GUI}/apps/iricgui/release/iRIC.exe
  )

# IRIC_DLLS
set(IRIC_DLLS
  ${PREPOST_GUI}/libs/dataitem/axis2d/release/iricAxis2d.dll
  ${PREPOST_GUI}/libs/dataitem/axis3d/release/iricAxis3d.dll
  ${PREPOST_GUI}/libs/cs/release/iricCs.dll
  ${PREPOST_GUI}/libs/dataitem/distancemeasure/release/iricDistancemeasure.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/centerandwidth/release/iricGccCenterandwidth.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/compoundchannel/release/iricGccCompoundchannel.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/externalprogram/release/iricGccExternalprogram.dll  
  ${PREPOST_GUI}/libs/gridcreatingcondition/gridcombine/release/iricGccGridcombine.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/gridgenerator/release/iricGccGridgenerator.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/laplace/release/iricGccLaplace.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/poisson/release/iricGccPoisson.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/rectangularregion/release/iricGccRectangularregion.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/rectangularregionlonlat/release/iricGccRectangularregionlonlat.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/riversurvey/release/iricGccRiversurvey.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/riversurvey15d/release/iricGccRiversurvey15d.dll
  ${PREPOST_GUI}/libs/gridcreatingcondition/triangle/release/iricGccTriangle.dll
  ${PREPOST_GUI}/libs/geodata/netcdf/release/iricGdNetcdf.dll
  ${PREPOST_GUI}/libs/geodata/pointmap/release/iricGdPointmap.dll
  ${PREPOST_GUI}/libs/geodata/polygon/release/iricGdPolygon.dll
  ${PREPOST_GUI}/libs/geodata/polyline/release/iricGdPolyLine.dll
  ${PREPOST_GUI}/libs/geodata/riversurvey/release/iricGdRiversurvey.dll
  ${PREPOST_GUI}/libs/geoio/release/iricGeoio.dll
  ${PREPOST_GUI}/libs/post/graph2d/release/iricGraph2d.dll
  ${PREPOST_GUI}/libs/post/graph2dhybrid/release/iricGraph2dhybrid.dll
  ${PREPOST_GUI}/libs/post/graph2dscattered/release/iricGraph2dscattered.dll
  ${PREPOST_GUI}/libs/gui/release/iricGui.dll
  ${PREPOST_GUI}/libs/guibase/release/iricGuibase.dll
  ${PREPOST_GUI}/libs/guicore/release/iricGuicore.dll
  ${PREPOST_GUI}/libs/hydraulicdata/riversurveywaterelevation/release/iricHdRiversurveywaterelevation.dll
  ${PREPOST_GUI}/libs/dataitem/measureddata/release/iricMeasureddata.dll
  ${PREPOST_GUI}/libs/misc/release/iricMisc.dll
  ${PREPOST_GUI}/libs/post/post2d/release/iricPost2d.dll
  ${PREPOST_GUI}/libs/post/post2dbirdeye/release/iricPost2dbirdeye.dll
  ${PREPOST_GUI}/libs/post/post3d/release/iricPost3d.dll
  ${PREPOST_GUI}/libs/postbase/release/iricPostbase.dll
  ${PREPOST_GUI}/libs/pre/release/iricPre.dll
  ${PREPOST_GUI}/libs/iricpython/release/iricPython.dll
  ${PREPOST_GUI}/libs/solverconsole/release/iricSolverconsole.dll
  ${PREPOST_GUI}/libs/tmsloader/release/iricTmsLoader.dll
  ${PREPOST_GUI}/libs/triangle/release/iricTriangle.dll
  )

# iRIC_ES
# dev_src/packages/gui.prepost/data/languages/iRIC_es_ES.qm ???
# dev_src/packages/gui.prepost/data/languages/qt_es_ES.qm ???

# IRIC_JP
# dev_src/packages/gui.prepost/data/languages/iRIC_ja_JP.qm ???
# dev_src/packages/gui.prepost/data/languages/qtbase_ja_JP.qm ???
set(IRIC_JP
  ${PREPOST_GUI}/libs/dataitem/axis2d/languages/iricAxis2d_ja_JP.qm
  ${PREPOST_GUI}/libs/dataitem/axis3d/languages/iricAxis3d_ja_JP.qm
  ${PREPOST_GUI}/libs/cs/languages/iricCs_ja_JP.qm
  ${PREPOST_GUI}/libs/dataitem/distancemeasure/languages/iricDistancemeasure_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/centerandwidth/languages/iricGccCenterandwidth_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/compoundchannel/languages/iricGccCompoundchannel_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/externalprogram/languages/iricGccExternalprogram_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/gridcombine/languages/iricGccGridcombine_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/gridgenerator/languages/iricGccGridgenerator_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/laplace/languages/iricGccLaplace_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/poisson/languages/iricGccPoisson_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/rectangularregion/languages/iricGccRectangularregion_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/rectangularregionlonlat/languages/iricGccRectangularregionlonlat_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/riversurvey/languages/iricGccRiversurvey_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/riversurvey15d/languages/iricGccRiversurvey15d_ja_JP.qm
  ${PREPOST_GUI}/libs/gridcreatingcondition/triangle/languages/iricGccTriangle_ja_JP.qm
  ${PREPOST_GUI}/libs/geodata/netcdf/languages/iricGdNetcdf_ja_JP.qm
  ${PREPOST_GUI}/libs/geodata/pointmap/languages/iricGdPointmap_ja_JP.qm
  ${PREPOST_GUI}/libs/geodata/polygon/languages/iricGdPolygon_ja_JP.qm
  ${PREPOST_GUI}/libs/geodata/polyline/languages/iricGdPolyLine_ja_JP.qm
  ${PREPOST_GUI}/libs/geodata/riversurvey/languages/iricGdRiversurvey_ja_JP.qm
  ${PREPOST_GUI}/libs/geoio/languages/iricGeoIO_ja_JP.qm
  ${PREPOST_GUI}/libs/post/graph2d/languages/iricGraph2d_ja_JP.qm
  ${PREPOST_GUI}/libs/post/graph2dhybrid/languages/iricGraph2dhybrid_ja_JP.qm
  ${PREPOST_GUI}/libs/post/graph2dscattered/languages/iricGraph2dscattered_ja_JP.qm
  ${PREPOST_GUI}/libs/gui/languages/iricGui_ja_JP.qm
  ${PREPOST_GUI}/libs/guibase/languages/iricGuibase_ja_JP.qm
  ${PREPOST_GUI}/libs/guicore/languages/iricGuicore_ja_JP.qm
  ${PREPOST_GUI}/libs/hydraulicdata/riversurveywaterelevation/languages/iricHdRiversurveywaterelevation_ja_JP.qm
  ${PREPOST_GUI}/libs/dataitem/measureddata/languages/iricMeasureddata_ja_JP.qm
  ${PREPOST_GUI}/libs/misc/languages/iricMisc_ja_JP.qm
  ${PREPOST_GUI}/libs/post/post2d/languages/iricPost2d_ja_JP.qm
  ${PREPOST_GUI}/libs/post/post2dbirdeye/languages/iricPost2dbirdeye_ja_JP.qm
  ${PREPOST_GUI}/libs/post/post3d/languages/iricPost3d_ja_JP.qm
  ${PREPOST_GUI}/libs/postbase/languages/iricPostbase_ja_JP.qm
  ${PREPOST_GUI}/libs/pre/languages/iricPre_ja_JP.qm
  ${PREPOST_GUI}/libs/solverconsole/languages/iricSolverconsole_ja_JP.qm
  ${PREPOST_GUI}/libs/gui/languages/qt_ja_JP.qm
  )
  
# IRIC_PY
set(IRIC_PY
  ${PREPOST_GUI}/apps/iricpython/iric.py
  )

# Install IRIC  
install(FILES ${IRIC_ICO} ${IRIC_EXE} ${IRIC_DLLS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gui.prepost/data
  )
install(FILES ${IRIC_JP}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gui.prepost/data/languages
  )
install(FILES ${IRIC_PY}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gui.prepost/data/pythonmodule
  )



# RIVMAKER_ICO
set(RIVMAKER_ICO
  ${PREPOST_GUI}/apps/rivmaker/images/iconRivMakerFile.ico
  )

# RIVMAKER_EXE
set(RIVMAKER_EXE
  ${PREPOST_GUI}/apps/rivmaker/release/Rivmaker.exe
  )

# RIVMAKER_DLLS
set(RIVMAKER_DLLS
  ${PREPOST_GUI}/libs/misc/release/iricMisc.dll
  ${PREPOST_GUI}/libs/triangle/release/iricTriangle.dll
  )

# RIVMAKER_JP
# dev_src/packages/gui.rivmaker/data/languages/qtbase_ja_JP.qm ???
set(RIVMAKER_JP
  ${PREPOST_GUI}/apps/rivmaker/languages/rivmaker_ja_JP.qm
  ${PREPOST_GUI}/libs/gui/languages/qt_ja_JP.qm
  )

# Install RIVMAKER  
#install(FILES ${RIVMAKER_ICO} ${RIVMAKER_EXE} ${RIVMAKER_DLLS}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gui.rivmaker/data
#  )
#install(FILES ${RIVMAKER_JP}
#  DESTINATION ${CMAKE_INSTALL_PREFIX}/dev_src/packages/gui.rivmaker/data/languages
#  )
