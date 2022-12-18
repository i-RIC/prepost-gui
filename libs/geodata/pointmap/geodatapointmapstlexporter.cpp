#include "geodatapointmapstlexporter.h"
#include "geodatapointmapt.h"

#include <cs/coordinatesystem.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/stringtool.h>

#include <QRegExp>

#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSTLWriter.h>

GeoDataPointmapSTLExporter::GeoDataPointmapSTLExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("Stereolithography (STL)"), creator)
{}

bool GeoDataPointmapSTLExporter::doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* /*w*/, ProjectData* pd)
{
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkPolyData* polydata = vtkPolyData::New();
	polydata->DeepCopy(pmap->delaunayedPolyData());

	double tmpvec[3];
	vtkIdType i, numpoints;
	numpoints = polydata->GetNumberOfPoints();
	vtkDoubleArray* val = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray("values"));
	auto offset = pd->mainfile()->offset();
	for (i = 0; i < numpoints; ++i) {
		polydata->GetPoints()->GetPoint(i, tmpvec);
		tmpvec[0] += offset.x();
		tmpvec[1] += offset.y();
		tmpvec[2] = val->GetValue(i);
		polydata->GetPoints()->SetPoint(i, tmpvec);
	}
	vtkSTLWriter* writer = vtkSTLWriter::New();
	if (selectedFilter == tr("Binary STL (*.stl)")) {
		writer->SetFileTypeToBinary();
	} else {
		writer->SetFileTypeToASCII();
	}
	writer->SetFileName(iRIC::toStr(filename).c_str());
	writer->SetInputData(polydata);
	writer->Write();

	writer->Delete();
	polydata->Delete();

	// export *.prj if project coordinate system is defined
	auto cs = pd->mainfile()->coordinateSystem();
	if (cs != nullptr) {
			auto prjFilename = filename;
			prjFilename.replace(QRegExp("\\.stl$"), ".prj");
			cs->exportPlaneWkt(prjFilename);
	}

	return true;
}

const QStringList GeoDataPointmapSTLExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ASCII STL (*.stl)");
	ret << tr("Binary STL (*.stl)");
	return ret;
}
