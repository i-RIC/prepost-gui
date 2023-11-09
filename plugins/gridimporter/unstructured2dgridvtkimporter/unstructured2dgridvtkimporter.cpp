#include "unstructured2dgridvtkimporter.h"

#include <guibase/vtkpointsetextended/vtkunstructuredgridextended2d.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridReader.h>

Unstructured2dGridVtkImporter::Unstructured2dGridVtkImporter() :
	QObject {nullptr},
	GridImporterI{}
{}

QStringList Unstructured2dGridVtkImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("VTK files (*.vtk)");
	return ret;
}

QString Unstructured2dGridVtkImporter::caption() const
{
	return tr("VTK file");
}

SolverDefinitionGridType::GridType Unstructured2dGridVtkImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtUnstructured2DGrid;
}

bool Unstructured2dGridVtkImporter::import(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
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

	auto reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
	reader->SetFileName(iRIC::toStr(tempFile).c_str());
	reader->Update();

	auto grid2d = dynamic_cast<v4Unstructured2dGrid*> (grid->grid());
	grid2d->vtkConcreteData()->concreteData()->DeepCopy(reader->GetOutput());

	QFile::remove(tempFile);

	grid->allocateAttributes();

	return true;
}
