#include "structured2dgridvtkimporter.h"

#include <guicore/pre/grid/structured2dgrid.h>
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
	GridImporterInterface{}
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

bool Structured2dGridVtkImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
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

	Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(grid);
	grid2d->vtkGrid()->DeepCopy(reader->GetOutput());
	grid2d->setDimensionsFromVtkGrid();

	QFile::remove(tempFile);

	// allocate memory for all grid related conditions.
	for (GridAttributeContainer* c : grid2d->gridAttributes()){
		c->allocate();
	}

	return true;
}
