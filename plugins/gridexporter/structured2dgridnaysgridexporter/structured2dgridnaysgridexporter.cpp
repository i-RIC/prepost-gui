#include "structured2dgridnaysgridexporter.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>

#include <QObject>
#include <QFile>
#include <QTextStream>

Structured2DGridNaysGridExporter::Structured2DGridNaysGridExporter() :
	QObject(), GridExporterI()
{}

QString Structured2DGridNaysGridExporter::caption() const
{
	return tr("RIC-Nays Grid file");
}

bool Structured2DGridNaysGridExporter::isGridTypeSupported(SolverDefinitionGridType::GridType gt) const
{
	return gt == SolverDefinitionGridType::gtStructured2DGrid;
}

QStringList Structured2DGridNaysGridExporter::fileDialogFilters() const
{
	QStringList ret;
	ret << QObject::tr("RIC-Nays grid files (*.grid)");
	return ret;
}

bool Structured2DGridNaysGridExporter::doExport(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	auto grid2d = dynamic_cast<v4Structured2dGrid*> (grid->grid());

	QFile f(filename);
	bool ret = f.open(QIODevice::WriteOnly);
	if (! ret){
		// file not opened successfully.
		return false;
	}
	QDataStream st(&f);
	st.setByteOrder(QDataStream::LittleEndian);

	auto c = grid->attribute("Elevation");
	if (c == nullptr){
		// this grid does not have Elevation. Impossible to export.
		return false;
	}
	auto container = dynamic_cast<GridAttributeRealContainer*>(c);
	if (container->gridAttribute()->position() != SolverDefinitionGridAttribute::Position::Node) {return false;}

	c = grid->attribute("CellCondition");
	auto cellcond = dynamic_cast<GridAttributeIntegerContainer*>(c);
	if (cellcond->gridAttribute()->position() != SolverDefinitionGridAttribute::Position::CellCenter) {return false;}

	bool cellcondexists = false;
	if (cellcond != nullptr){
		for (unsigned int i = 0; (! cellcondexists) && i < grid2d->dimensionI() - 1; ++i){
			for (unsigned int j = 0; (! cellcondexists) && j < grid2d->dimensionJ() - 1; ++j){
				cellcondexists = cellcondexists || (cellcond->value(grid2d->cellIndex(i, j)) != 0);
			}
		}
	}

	st << (int) (16) << grid2d->dimensionI() << grid2d->dimensionJ() << (int) (2) << int (cellcondexists) << int (16);

	// load the size of X, Y, Z data. this is needless.
	int datasize = grid2d->dimensionI() * grid2d->dimensionJ();
	st << datasize * 2 * 3 * 8;

	char dummybuffer[8];
	double v[3];
	// write x coordinates. written twice (for k = 0 and 1).
	for (int j = 0; j < 2; ++j){
		for (int i = 0; i < datasize; ++i){
			grid2d->vtkConcreteData()->concreteData()->GetPoint(i, v);
			memcpy(dummybuffer, &(v[0]), 8);
			st.writeRawData(dummybuffer, 8);
		}
	}
	// write y coordinates. written twice (for k = 0 and 1).
	for (int j = 0; j < 2; ++j){
		for (int i = 0; i < datasize; ++i){
			grid2d->vtkConcreteData()->concreteData()->GetPoint(i, v);
			memcpy(dummybuffer, &(v[1]), 8);
			st.writeRawData(dummybuffer, 8);
		}
	}
	// write z coordinates. written twice (for k = 0 and 1).
	for (int j = 0; j < 2; ++j){
		for (int i = 0; i < datasize; ++i){
			double dummydbl = container->value(i);
			memcpy(dummybuffer, &(dummydbl), 8);
			st.writeRawData(dummybuffer, 8);
		}
	}
	st << datasize * 2 * 3 * 8;
	if (cellcondexists){
		datasize = (grid2d->dimensionI() - 1) * (grid2d->dimensionJ() - 1);
		st << datasize * static_cast<int>(sizeof (long));
		for (int i = 0; i < datasize; ++i){
			int val = cellcond->value(i);
			memcpy(dummybuffer, &val, 4);
			st.writeRawData(dummybuffer, 4);
		}
		st << datasize * static_cast<int>(sizeof (long));
	}
	return true;
}
