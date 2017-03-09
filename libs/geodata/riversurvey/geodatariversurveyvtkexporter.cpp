#include "geodatariversurvey.h"
#include "geodatariversurveyvtkexporter.h"

#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QStringList>

#include <vtkStructuredGridWriter.h>

GeoDataRiverSurveyVTKExporter::GeoDataRiverSurveyVTKExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("RiverSurvey data"), creator}
{}

bool GeoDataRiverSurveyVTKExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* /*pd*/)
{
	QString tempPath = QDir::tempPath();
	QString tmpFile = iRIC::getTempFileName(tempPath);

	GeoDataRiverSurvey* rsurvey = dynamic_cast<GeoDataRiverSurvey*>(data);
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

const QStringList GeoDataRiverSurveyVTKExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("VTK File (*.vtk)");
	return ret;
}
