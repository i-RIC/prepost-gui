#include "structured2dgridnaysgridimporter.h"

#include <float.h>
#include <cmath>

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>

#include <QDataStream>
#include <QFile>
#include <QMessageBox>
#include <QObject>

#include <vector>

Structured2DGridNaysGridImporter::Structured2DGridNaysGridImporter() :
	QObject(), GridImporterI()
{}

QString Structured2DGridNaysGridImporter::caption() const
{
	return tr("RIC-Nays Grid file");
}

QStringList Structured2DGridNaysGridImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("RIC-Nays grid file (*.grid)");
	return ret;
}

SolverDefinitionGridType::GridType Structured2DGridNaysGridImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

bool Structured2DGridNaysGridImporter::import(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	// Show warning dialog first.
	QMessageBox::warning(parent, tr("Warning"), tr("Cell flag values will not be loaded."), QMessageBox::Ok);

	auto grid2d = dynamic_cast<v4Structured2dGrid*>(grid->grid());

	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream st(&f);
	st.setByteOrder(QDataStream::LittleEndian);

	GridAttributeContainer* c = grid->attribute("Elevation");
	if (c == 0){
		// this grid does not have Elevation. Impossible to import.
		return false;
	}
	auto container = dynamic_cast<GridAttributeRealContainer*>(c);

	int len, imax, jmax, kmax, obst;
	// Load imax, jmax, kmax, and obst flag.
	st >> len >> imax >> jmax >> kmax >> obst >> len;

	// load the size of X, Y, Z data. this is needless.
	st >> len;

	// kmax == 2 always. data in k = 0 should be loaded.

	grid2d->setDimensions(imax, jmax);
	grid->allocateAttributes();

	int gridsize = imax * jmax;
	std::vector<double> x(gridsize);
	std::vector<double> y(gridsize);
	std::vector<double> z(gridsize);

	char dummybuffer[8];

	for (int i = 0; i < gridsize; ++i){
		st.readRawData(dummybuffer, 8);
		memcpy(&(x[i]), dummybuffer, 8);
		// check for NaN, inf
		if (std::isfinite(x[i]) == 0) {return false;}
	}
	// x data for k = 1 are trashed.
	for (int i = 0; i < gridsize; ++i){
		st.readRawData(dummybuffer, 8);
	}

	for (int i = 0; i < gridsize; ++i){
		st.readRawData(dummybuffer, 8);
		memcpy(&(y[i]), dummybuffer, 8);
		// check for NaN, inf
		if (std::isfinite(y[i]) == 0) {return false;}
	}
	// y data for k = 1 are trashed.
	for (int i = 0; i < gridsize; ++i){
		st.readRawData(dummybuffer, 8);
	}
	for (int i = 0; i < gridsize; ++i){
		st.readRawData(dummybuffer, 8);
		memcpy(&(z[i]), dummybuffer, 8);
		// check for NaN, inf
		if (std::isfinite(z[i]) == 0) {return false;}
	}
	// z data for k = 1 are trashed.
	for (int i = 0; i < gridsize; ++i){
		st.readRawData(dummybuffer, 8);
	}

	int offset = 0;
	for (int j = 0; j < jmax; ++j){
		for (int i = 0; i < imax; ++i){
			grid2d->setPoint2d(i, j, QPointF(x[offset], y[offset]));
			container->setValue(grid2d->pointIndex(i, j), z[offset]);
			++offset;
		}
	}

	return true;
}
