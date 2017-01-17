#include "geodatapointmapt.h"
#include "geodatapointmapwebimporter.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

GeoDataPointmapWebImporter::GeoDataPointmapWebImporter(GeoDataCreator* creator) :
	GeoDataWebImporter("csvtile", tr("Web CSV tile"), creator),
	m_coordinateSystem {nullptr}
{}

bool GeoDataPointmapWebImporter::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (condition->name() == "Elevation") {return true;}

	return false;
}

bool GeoDataPointmapWebImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	// @todo implement this
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkDoubleArray> values = vtkSmartPointer<vtkDoubleArray>::New();

	points->InsertNextPoint(0, 0, 0);
	points->InsertNextPoint(1, 0, 0);
	points->InsertNextPoint(0, 1, 0);
	points->InsertNextPoint(1, 1, 0);

	values->InsertNextValue(0);
	values->InsertNextValue(0);
	values->InsertNextValue(0);
	values->InsertNextValue(0);

	pmap->setPoints(points, values);
	pmap->doDelaunay();

	return true;
}

bool GeoDataPointmapWebImporter::doInit(int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	*count = 1;
	auto mainfile = item->projectData()->mainfile();
	if (mainfile->coordinateSystem() == nullptr) {
		int dialogRet = mainfile->showCoordinateSystemDialog();
		if (dialogRet == QDialog::Rejected) {
			return false;
		}
	}
	m_coordinateSystem = mainfile->coordinateSystem();

	// @todo show dialog and setup import setting here.

	return true;
}
