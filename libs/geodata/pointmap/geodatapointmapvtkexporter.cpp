#include "geodatapointmapt.h"
#include "geodatapointmapvtkexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QStringList>

#include <vtkDoubleArray.h>
#include <vtkPolyDataWriter.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

GeoDataPointmapVTKExporter::GeoDataPointmapVTKExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("Pointset (DEM etc.)"), creator)
{}

bool GeoDataPointmapVTKExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	QString tempPath = QDir::tempPath();
	QString tmpFile = iRIC::getTempFileName(tempPath);

	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkPolyData* polyData = pmap->delaunayedPolyData();

	// undo offset using transform
	auto offset = pd->mainfile()->offset();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Translate(offset.x(), offset.y(), 0.0);
	vtkSmartPointer<vtkTransformFilter> filter = vtkSmartPointer<vtkTransformFilter>::New();
	filter->SetInputData(polyData);
	filter->SetTransform(transform);

	vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
	writer->SetInputConnection(filter->GetOutputPort());
	writer->SetFileTypeToBinary();
	writer->SetFileName(iRIC::toStr(tmpFile).c_str());

	// export data.
	writer->Update();
	writer->Delete();

	// rename the temporary file to the target file.
	if (QFile::exists(filename)) {
		// remove first.
		if (! QFile::remove(filename)) {
			// unable to remove. fail.
			QFile::remove(tmpFile);
			return false;
		}
	}
	bool ok = QFile::rename(tmpFile, filename);
	if (! ok) {
		// rename failed.
		QFile::remove(tmpFile);
		return false;
	}
	return true;
}

const QStringList GeoDataPointmapVTKExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("VTK File (*.vtk)");
	return ret;
}
