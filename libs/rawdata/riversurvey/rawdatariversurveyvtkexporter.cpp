#include "rawdatariversurvey.h"
#include "rawdatariversurveyvtkexporter.h"

#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QStringList>

#include <vtkStructuredGridWriter.h>

RawDataRiverSurveyVTKExporter::RawDataRiverSurveyVTKExporter(RawDataCreator* creator)
	: RawDataExporter(creator)
{
	m_caption = tr("RiverSurvey data");
}

bool RawDataRiverSurveyVTKExporter::doEmport(RawData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	QString tempPath = QDir::tempPath();
	QString tmpFile = iRIC::getTempFileName(tempPath);

	RawDataRiverSurvey* rsurvey = dynamic_cast<RawDataRiverSurvey*>(data);
	vtkStructuredGrid* grid = rsurvey->backgroundGrid();

	vtkStructuredGridWriter* writer = vtkStructuredGridWriter::New();
	writer->SetInputData(grid);
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

const QStringList RawDataRiverSurveyVTKExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("VTK File (*.vtk)");
	return ret;
}
