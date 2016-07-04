#include <iricpython/application.h>
#include <iricpython/calculationresult.h>
#include <iricpython/geodatagroup.h>
#include <iricpython/geodataimporter.h>
#include <iricpython/pre.h>
#include <iricpython/solver.h>

#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	iRICPython::Application::init("E:/iricdev_2013/src_git/apps/iricgui/debug");

	iRICPython::Application app;

	// open project
	app.openProject("C:/iricpython/projects/nays2dflood.ipro");

	// import xband data
	iRICPython::GeoDataGroup* xrainGroup = app.pre()->geoDataGroup("Xrain");
	iRICPython::GeoDataImporter* importer = xrainGroup->importer("xband");
	iRICPython::GeoData* data = importer->importGeoData("C:/iricpython/xraindata/TOUKYOU001-20150111-1119-G000-EL000000");
	xrainGroup->add(data);

	// map xband data to grid
	app.pre()->mapGeoData("Xrain");

	// save project
	app.saveProjectAs("C:/iricpython/projects/nays2dflood2.ipro");

	// run solver
	app.solver()->startAndWaitForFinish();

	// save project	again
	app.saveProjectAs("C:/iricpython/projects/nays2dflood3.ipro");

	// export calculation result
	std::vector<int> steps;
	int resultLen = app.calcResult()->length();
	for (int i = 0; i < resultLen; ++i) {
		steps.push_back(i);
	}
	app.calcResult()->exportToFolder("C:/iricpython/exportresult", "Result_", steps, iRICPython::CalculationResult::ExportType::ESRIShape);

	return 0;
}
