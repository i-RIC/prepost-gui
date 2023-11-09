#include "unstructured2dgridvtkexporter.h"
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QStringList>
#include <QFile>
#include <QDir>

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridWriter.h>

Unstructured2DGridVTKExporter::Unstructured2DGridVTKExporter() :
	QObject{nullptr},
	GridExporterI {}
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

bool Unstructured2DGridVTKExporter::doExport(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	QString tempPath = QDir::tempPath();
	QString tempFile = iRIC::getTempFileName(tempPath);
	QFile f(filename);
	f.copy(tempFile);

	auto grid2d = dynamic_cast<v4Unstructured2dGrid*> (grid->grid());
	auto writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
	writer->SetInputData(grid2d->vtkConcreteData()->concreteData());
	writer->SetFileTypeToASCII();
	writer->SetFileName(iRIC::toStr(tempFile).c_str());

	// export data.
	writer->Update();

	// rename the temporary file to the target file.
	if (QFile::exists(filename)){
		// remove first.
		if (! QFile::remove(filename)){
			// unable to remove. fail.
			QFile::remove(tempFile);
			return false;
		}
	}
	bool ok = QFile::rename(tempFile, filename);
	if (! ok){
		// rename failed.
		QFile::remove(tempFile);
		return false;
	}
	return true;
}
