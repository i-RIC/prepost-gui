#include "structured2dgridnayscsvexporter.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercellcontainer.h>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QVector2D>

Structured2DGridNaysCSVExporter::Structured2DGridNaysCSVExporter() :
	QObject()
{}

Structured2DGridNaysCSVExporter::~Structured2DGridNaysCSVExporter()
{}

QString Structured2DGridNaysCSVExporter::caption() const
{
	return tr("RIC-Nays CSV");
}

bool Structured2DGridNaysCSVExporter::isGridTypeSupported(SolverDefinitionGridType::GridType gt) const
{
	return gt == SolverDefinitionGridType::gtStructured2DGrid;
}

QStringList Structured2DGridNaysCSVExporter::fileDialogFilters() const
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
	if (! f.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text)){return false;}

	GridAttributeContainer* c = grid2d->gridAttribute("Elevation");
	if (c == nullptr){
		// this grid does not have Elevation node. Impossible to export data.
		return false;
	}
	GridAttributeRealNodeContainer* container = dynamic_cast<GridAttributeRealNodeContainer*>(c);

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
	f.close();
	return true;
}
