#include "geodatapointmaprealexporter.h"
#include "geodatapointmapt.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

#include <vtkDoubleArray.h>

GeoDataPointmapRealExporter::GeoDataPointmapRealExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("Pointset (DEM etc.)"), creator)
{}

bool GeoDataPointmapRealExporter::doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd)
{
	// Allocate objects to hold points and vertex cells.
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	if (selectedFilter == tr("Topography File (*.tpo)")) {
		QFile file(filename);
		if (! file.open(QIODevice::WriteOnly)) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while exporting to %1.").arg(filename));
			return false;
		}
		QTextStream outs(&file);
		outs.setRealNumberNotation(QTextStream::SmartNotation);
		outs.setRealNumberPrecision(10);

		outs << pmap->vtkGrid()->GetPoints()->GetNumberOfPoints() << endl;
		vtkPoints* points = pmap->vtkGrid()->GetPoints();
		vtkDoubleArray* values = vtkDoubleArray::SafeDownCast(pmap->vtkGrid()->GetPointData()->GetArray("values"));
		auto offset = pd->mainfile()->offset();
		double v[3], val;
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, v);
			val = values->GetValue(i);
			v[0] += offset.x();
			v[1] += offset.y();
			outs << v[0] << " " << v[1] << " " << val << endl;
		}
		file.close();
	}
	return true;
}

const QStringList GeoDataPointmapRealExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Topography File (*.tpo)");
	return ret;
}
