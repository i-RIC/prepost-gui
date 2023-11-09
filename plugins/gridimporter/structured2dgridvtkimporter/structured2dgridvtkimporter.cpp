#include "structured2dgridvtkimporter.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGridReader.h>

Structured2dGridVtkImporter::Structured2dGridVtkImporter() :
	QObject {nullptr},
	GridImporterI{}
{}

QStringList Structured2dGridVtkImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("VTK files (*.vtk)");
	return ret;
}

QString Structured2dGridVtkImporter::caption() const
{
	return tr("VTK file");
}

SolverDefinitionGridType::GridType Structured2dGridVtkImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

bool Structured2dGridVtkImporter::import(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	QString tempPath = QDir::tempPath();
	QString tempFile = iRIC::getTempFileName(tempPath);
	QFile f(filename);
	bool ok = f.copy(tempFile);
	if (! ok) {
		QMessageBox::critical(parent, tr("Error"), tr("Copying %1 to %2 failed.")
													.arg(QDir::toNativeSeparators(filename))
													.arg(QDir::toNativeSeparators(tempFile)));
		return false;
	}

	auto reader = vtkSmartPointer<vtkStructuredGridReader>::New();
	reader->SetFileName(iRIC::toStr(tempFile).c_str());
	reader->Update();

	auto grid2d = dynamic_cast<v4Structured2dGrid*> (grid->grid());

	auto importedGrid = reader->GetOutput();
	int dims[3];
	importedGrid->GetDimensions(dims);
	grid2d->setDimensions(dims[0], dims[1]);
	grid2d->vtkConcreteData()->concreteData()->DeepCopy(importedGrid);

	QFile::remove(tempFile);

	grid->allocateAttributes();
	return true;
}
