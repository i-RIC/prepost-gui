#include "geodatapointmaprealexporter.h"
#include "geodatapointmapt.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/tpoexporter.h>

#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

#include <vtkDoubleArray.h>

GeoDataPointmapRealExporter::GeoDataPointmapRealExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("Point Cloud Data (DEM etc.)"), creator)
{}

bool GeoDataPointmapRealExporter::doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd)
{
	// Allocate objects to hold points and vertex cells.
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	if (selectedFilter == tr("Topography File (*.tpo)")) {
		TpoExporter exporter(w);
		if (! exporter.open(filename)) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while exporting to %1.").arg(filename));
			return false;
		}

		exporter.setOffset(pd->mainfile()->offset());

		vtkPoints* points = pmap->vtkGrid()->GetPoints();
		vtkDoubleArray* values = vtkDoubleArray::SafeDownCast(pmap->vtkGrid()->GetPointData()->GetArray("values"));

		double v[3], val;
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, v);
			val = values->GetValue(i);
			exporter.addPoint(v[0], v[1], val);
		}
		exporter.close();
	}
	return true;
}

const QStringList GeoDataPointmapRealExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Topography File (*.tpo)");
	return ret;
}
