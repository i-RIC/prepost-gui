#include "rawdatapointmaprealexporter.h"
#include "rawdatapointmapt.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

#include <vtkDoubleArray.h>

RawDataPointmapRealExporter::RawDataPointmapRealExporter(RawDataCreator* creator)
	: RawDataExporter(creator)
{
	m_caption = tr("Pointset (DEM etc.)");
}

bool RawDataPointmapRealExporter::doExport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd)
{
	// Allocate objects to hold points and vertex cells.
	RawDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<RawDataPointMapT<double, vtkDoubleArray>*>(data);
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
		QVector2D offset = pd->mainfile()->offset();
		double v[3], val;
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, v);
			val = values->GetValue(i);
			v[0] += offset.x();
			v[1] += offset.y();
			outs << v[0] << " " << v[1] << " " << val << endl;
		}
		file.close();
	} else if (selectedFilter == tr("RIC-Nays DEM (*.dat)")) {
		QFile file(filename);
		if (! file.open(QIODevice::WriteOnly)) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while exporting to %1.").arg(filename));
			return false;
		}
		QTextStream outs(&file);
		outs.setRealNumberNotation(QTextStream::SmartNotation);
		outs.setRealNumberPrecision(10);
		vtkPoints* points = pmap->vtkGrid()->GetPoints();
		vtkDoubleArray* values = vtkDoubleArray::SafeDownCast(pmap->vtkGrid()->GetPointData()->GetArray("values"));
		QVector2D offset = pd->mainfile()->offset();
		double v[3], val;
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, v);
			val = values->GetValue(i);
			v[0] += offset.x();
			v[1] += offset.y();
			outs << (i + 1) << "," << v[0] << "," << v[1] << "," << val << endl;
		}
		file.close();
	}
	return true;
}

const QStringList RawDataPointmapRealExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Topography File (*.tpo)");
	ret << tr("RIC-Nays DEM (*.dat)");
	return ret;
}
