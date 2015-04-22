#include "structured2dgridnayscsvexporter.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridrelatedconditionrealnodecontainer.h>
#include <guicore/pre/gridcond/container/gridrelatedconditionintegercellcontainer.h>

#include <QObject>
#include <QFile>
#include <QTextStream>

Structured2DGridNaysCSVExporter::Structured2DGridNaysCSVExporter()
	: GridExporterInterface(), QObject()
{

}

QString Structured2DGridNaysCSVExporter::caption() const
{
	return tr("RIC-Nays CSV");
}

const QStringList Structured2DGridNaysCSVExporter::fileDialogFilters()
{
	QStringList ret;
	ret << QObject::tr("iRIC Grid CSV files (*.csv)");
	return ret;
}

bool Structured2DGridNaysCSVExporter::doExport(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*parent*/)
{
	Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(grid);
	int imax = grid2d->dimensionI();
	int jmax = grid2d->dimensionJ();
	int kmax = 2;

	QFile f(filename);
	if (f.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text)){
		GridRelatedConditionContainer* c = grid2d->gridRelatedCondition("Elevation");
		if (c == nullptr){
			// this grid does not have Elevation node. Impossible to export data.
			return false;
		}
		GridRelatedConditionRealNodeContainer* container = dynamic_cast<GridRelatedConditionRealNodeContainer*>(c);

		c = grid2d->gridRelatedCondition("CellCondition");
		GridRelatedConditionIntegerCellContainer* cellcond = dynamic_cast<GridRelatedConditionIntegerCellContainer*>(c);
		QTextStream o(&f);
		o.setRealNumberPrecision(8);
		o << "IMAX,JMAX,KMAX " << endl;
		o << imax << "," << jmax << "," << kmax << endl;

		o << "I,J,K,X,Y,Z" << endl;
		o << scientific;
		for (int k = 0; k < kmax; k++){
			for (int j = 0; j < jmax; j++){
				for (int i = 0; i < imax; i++){
					o << i << "," << j << "," << k << ","
					  << grid2d->vertex(i, j).x() << ","
					  << grid2d->vertex(i, j).y() << ","
					  << container->value(grid2d->vertexIndex(i, j)) << endl;
				}
			}
		}
		if (cellcond != 0){
			o << "I , J , K , FLAGS" << endl;
			for (int k = 0; k < kmax - 1; k++){
				for (int j = 0; j < jmax - 1; j++){
					for (int i = 0; i < imax - 1; i++){
						o << i << "," << j << "," << k << ","
						  << cellcond->value(grid2d->cellIndex(i, j)) << endl;
					}
				}
			}
		}
	}else{
		// file not opened successfully.
		return false;
	}
	f.close();
	return true;
}
