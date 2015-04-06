#include "rawdatapointmapt.h"
#include "rawdatapointmapvtkexporter.h"

#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QDir>
#include <QFile>
#include <QStringList>

#include <vtkDoubleArray.h>
#include <vtkPolyDataWriter.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

RawDataPointmapVTKExporter::RawDataPointmapVTKExporter(RawDataCreator* creator)
	: RawDataExporter(creator)
{
	m_caption = tr("Pointset (DEM etc.)");
}

bool RawDataPointmapVTKExporter::doEmport(RawData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	QString tempPath = QDir::tempPath();
	QString tmpFile = iRIC::getTempFileName(tempPath);

	RawDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<RawDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkPolyData* polyData = pmap->delaunayedPolyData();

	// undo offset using transform
	QVector2D offset = pd->mainfile()->offset();
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
	if (QFile::exists(filename)){
		// remove first.
		if (! QFile::remove(filename)){
			// unable to remove. fail.
			QFile::remove(tmpFile);
			return false;
		}
	}
	bool ok = QFile::rename(tmpFile, filename);
	if (! ok){
		// rename failed.
		QFile::remove(tmpFile);
		return false;
	}
	return true;
}

const QStringList RawDataPointmapVTKExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("VTK File (*.vtk)");
	return ret;
}
