#include "structured2dgridnaysgridimporter.h"

#include <float.h>

#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridrelatedconditionintegercellcontainer.h>
#include <guicore/pre/gridcond/container/gridrelatedconditionrealnodecontainer.h>

#include <QDataStream>
#include <QFile>
#include <QMessageBox>
#include <QObject>

Structured2DGridNaysGridImporter::Structured2DGridNaysGridImporter()
	: GridImporterInterface(), QObject()
{

}

QString Structured2DGridNaysGridImporter::caption() const
{
	return tr("RIC-Nays Grid file");
}

const QStringList Structured2DGridNaysGridImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("RIC-Nays grid file (*.grid)");
	return ret;
}

bool Structured2DGridNaysGridImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	// Show warning dialog first.
	QMessageBox::warning(parent, tr("Warning"), tr("Cell flag values will not be loaded."), QMessageBox::Ok);

	Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(grid);

	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream st(&f);
	st.setByteOrder(QDataStream::LittleEndian);

	GridRelatedConditionContainer* c = grid2d->gridRelatedCondition("Elevation");
	if (c == 0){
		// this grid does not have Elevation. Impossible to import.
		return false;
	}
	GridRelatedConditionRealNodeContainer* container = dynamic_cast<GridRelatedConditionRealNodeContainer*>(c);

	int len, imax, jmax, kmax, obst;
	// Load imax, jmax, kmax, and obst flag.
	st >> len >> imax >> jmax >> kmax >> obst >> len;

	// load the size of X, Y, Z data. this is needless.
	st >> len;

	// kmax == 2 always. data in k = 0 should be loaded.

	grid2d->setDimensions(imax, jmax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate(imax * jmax);

	points->InsertPoint(imax * jmax - 1, 0, 0, 0);
	grid2d->vtkGrid()->SetPoints(points);

	// allocate memory for all grid related conditions.
	QList<GridRelatedConditionContainer*>& clist = grid2d->gridRelatedConditions();
	for (auto it = clist.begin(); it != clist.end(); ++it){
		(*it)->allocate();
	}
	double *x = new double[imax * jmax];
	double *y = new double[imax * jmax];
	double *z = new double[imax * jmax];

	char dummybuffer[8];
	for (int i = 0; i < imax * jmax; ++i){
		st.readRawData(dummybuffer, 8);
		memcpy((x + i), dummybuffer, 8);
		// check for NaN, inf
		if (_finite(*(x + i)) == 0){
			goto ERROR;
		}
	}
	// x data for k = 1 are trashed.
	for (int i = 0; i < imax * jmax; ++i){
		st.readRawData(dummybuffer, 8);
	}

	for (int i = 0; i < imax * jmax; ++i){
		st.readRawData(dummybuffer, 8);
		memcpy((y + i), dummybuffer, 8);
		// check for NaN, inf
		if (_finite(*(y + i)) == 0){
			goto ERROR;
		}
	}
	// y data for k = 1 are trashed.
	for (int i = 0; i < imax * jmax; ++i){
		st.readRawData(dummybuffer, 8);
	}
	for (int i = 0; i < imax * jmax; ++i){
		st.readRawData(dummybuffer, 8);
		memcpy((z + i), dummybuffer, 8);
		// check for NaN, inf
		if (_finite(*(z + i)) == 0){
			goto ERROR;
		}
	}
	// z data for k = 1 are trashed.
	for (int i = 0; i < imax * jmax; ++i){
		st.readRawData(dummybuffer, 8);
	}

	int offset = 0;
	for (int j = 0; j < jmax; ++j){
		for (int i = 0; i < imax; ++i){
			unsigned int id = grid2d->vertexIndex(i, j);
			points->InsertPoint(id, *(x + offset), *(y + offset), 0);
			container->setValue(id, *(z + offset));
			++offset;
		}
	}
	delete x;
	delete y;
	delete z;
	return true;

ERROR:
	delete x;
	delete y;
	delete z;
	return false;
}
