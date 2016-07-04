import iric

iric.Application.init("C:/iricpython/gui/prepost")
app = iric.Application()

# open project
app.openProject("C:/iricpython/projects/nays2dflood.ipro")

# import xrain data
xrainGroup = app.pre().geoDataGroup("Xrain")
importer = xrainGroup.importer("xband")
data = importer.importGeoData("C:/iricpython/xraindata/TOUKYOU001-20150111-1119-G000-EL000000")
xrainGroup.add(data)

# map xrain data to grid
app.pre().mapGeoData("Xrain")

# save project
app.saveProjectAs("C:/iricpython/projects/nays2dflood2.ipro")

# run solver
app.solver().startAndWaitForFinish()

# save project	again
app.saveProjectAs("C:/iricpython/projects/nays2dflood3.ipro")

# export result to ESRI shape files
resultLen = app.calcResult().length()
steps = range(0, resultLen - 1)
app.calcResult().exportToFolder("C:/iricpython/exportresult", "Result_", steps, iric.CalculationResult.ExportType_ESRIShape)
