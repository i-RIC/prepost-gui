#include "unstructured2dgridvtkexporter.h"
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QStringList>
#include <QFile>
#include <QDir>

#include <vtkUnstructuredGridWriter.h>

Unstructured2DGridVTKExporter::Unstructured2DGridVTKExporter() :
	QObject{nullptr},
	GridExporterInterface {}
{}

QString Unstructured2DGridVTKExporter::caption() const
{
	return tr("VTK file");
}

bool Unstructured2DGridVTKExporter::isGridTypeSupported(SolverDefinitionGridType::GridType gt) const
{
	return gt == SolverDefinitionGridType::gtUnstructured2DGrid;
}

QStringList Unstructured2DGridVTKExporter::fileDialogFilters() const
{
	QStringList ret;
	ret << QObject::tr("VTK files (*.vtk)");
	return ret;
}

bool Unstructured2DGridVTKExporter::doExport(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	QString tempPath = QDir::tempPath();
	QString tmpFile = iRIC::getTempFileName(tempPath);

	vtkUnstructuredGridWriter* writer = vtkUnstructuredGridWriter::New();
	writer->SetInputData(vtkUnstructuredGrid::SafeDownCast(grid->vtkGrid()));
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
