#include "unstructured2dgridtriangleimporter.h"

#include <guicore/pre/grid/unstructured2dgrid.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>

Unstructured2dGridTriangleImporter::Unstructured2dGridTriangleImporter()
	: GridImporterInterface(), QObject(0)
{

}

QStringList Unstructured2dGridTriangleImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("TRIANGLE node files (*.node)");
	return ret;
}

QString Unstructured2dGridTriangleImporter::caption() const
{
	return tr("TRIANGLE output (*.node, *.ele)");
}

bool Unstructured2dGridTriangleImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	Unstructured2DGrid* grid2d = dynamic_cast<Unstructured2DGrid*>(grid);

	QFileInfo nodefinfo(filename);

	QString nodefilename = nodefinfo.fileName();
	QString elefilename = nodefilename.left(nodefilename.length() - 5).append(".ele");
	QString elefullfilename = QDir(nodefinfo.absolutePath()).absoluteFilePath(elefilename);

	// check whether both .node file and .ele file exists.
	QFile nodef(filename);
	QFile elef(elefullfilename);
	if (! nodef.exists()){
		QMessageBox::warning(parent, tr("Warning"), tr("%1 does not exists.").arg(nodef.fileName()));
		return false;
	}
	if (! elef.exists()){
		QMessageBox::warning(parent, tr("Warning"), tr("%1 does not exists.").arg(elef.fileName()));
		return false;
	}
	// read .node file.
	nodef.open(QIODevice::ReadOnly | QIODevice::Truncate | QIODevice::Text);
	QTextStream ts(&nodef);

	int numNode, dim, numAtts, bMarkers;
	// read header.
	ts >> numNode >> dim >> numAtts >> bMarkers;

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	points->Allocate(numNode);
	for (int i = 0; i < numNode; ++i){
		int index;
		double x, y, attribute;
		int boundary;
		ts >> index >> x >> y;
		for (int j = 0; j < numAtts; ++j){
			ts >> attribute;  // load attribute. It is discarded.
		}
		ts >> boundary; // load boundary (0 or 1). It is discarded.
		points->InsertNextPoint(x, y, 0);
	}

	vtkUnstructuredGrid* ugrid = grid2d->vtkGrid();
	ugrid->SetPoints(points);
	// Close *.node file
	nodef.close();

	// read .ele file
	elef.open(QIODevice::ReadOnly | QIODevice::Truncate | QIODevice::Text);
	ts.setDevice(&elef);

	int numTri, nodesPerTri;
	// read header.
	ts >> numTri >> nodesPerTri >> numAtts;
	vtkSmartPointer<vtkTriangle> triangle;
	for (int i = 0; i < numTri; ++i){
		int index, v0, v1, v2;
		ts >> index >> v0 >> v1 >> v2;
		for (int j = 0; j < numAtts; ++j){
			double attribute;
			ts >> attribute;  // Load attribute. It is discarded.
		}
		triangle = vtkSmartPointer<vtkTriangle>::New();
		triangle->GetPointIds()->SetId(0, v0 - 1);
		triangle->GetPointIds()->SetId(1, v1 - 1);
		triangle->GetPointIds()->SetId(2, v2 - 1);
		ugrid->InsertNextCell(triangle->GetCellType(), triangle->GetPointIds());
	}
	// Close *.ele
	elef.close();
	ugrid->Modified();
	ugrid->BuildLinks();

	// allocate memory for all grid related conditions.
	for (GridRelatedConditionContainer* c : grid2d->gridRelatedConditions()){
		c->allocate();
	}

	return true;
}
