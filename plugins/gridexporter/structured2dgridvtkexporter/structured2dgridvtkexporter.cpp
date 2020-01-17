#include "structured2dgridvtkexporter.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QStringList>
#include <QFile>
#include <QDir>

#include <vtkStructuredGridWriter.h>

Structured2DGridVTKExporter::Structured2DGridVTKExporter() :
	QObject()
{}

Structured2DGridVTKExporter::~Structured2DGridVTKExporter()
{}

QString Structured2DGridVTKExporter::caption() const
{
	return tr("VTK file");
}

bool Structured2DGridVTKExporter::isGridTypeSupported(SolverDefinitionGridType::GridType gt) const
{
	return gt == SolverDefinitionGridType::gtStructured2DGrid;
}

QStringList Structured2DGridVTKExporter::fileDialogFilters() const
{
	QStringList ret;
	ret << QObject::tr("VTK files (*.vtk)");
	return ret;
}

bool Structured2DGridVTKExporter::doExport(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	QString tempPath = QDir::tempPath();
	QString tmpFile = iRIC::getTempFileName(tempPath);

	vtkStructuredGridWriter* writer = vtkStructuredGridWriter::New();
	writer->SetInputData(vtkStructuredGrid::SafeDownCast(grid->vtkGrid()));
	writer->SetFileTypeToASCII();
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
