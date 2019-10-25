#include "structured2dgridnayscsvimporter.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QVector2D>
#include <QString>
#include <QMessageBox>
#include <sstream>

Structured2DGridNaysCSVImporter::Structured2DGridNaysCSVImporter()
	: GridImporterInterface(), QObject()
{

}

QString Structured2DGridNaysCSVImporter::caption() const
{
	return tr("iRIC Grid CSV file");
}

SolverDefinitionGridType::GridType Structured2DGridNaysCSVImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

QStringList Structured2DGridNaysCSVImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("iRIC Grid CSV files (*.csv)");
	return ret;
}

bool Structured2DGridNaysCSVImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(grid);

	QFile f(filename);
	if (f.open(QFile::ReadOnly | QFile::Truncate | QIODevice::Text)){
		GridAttributeContainer* c = grid2d->gridAttribute("Elevation");
		if (c == 0){
			// this grid does not have elevation. Impossible to import.
			f.close();
			return false;
		}
		GridAttributeRealNodeContainer* container = dynamic_cast<GridAttributeRealNodeContainer*>(c);

		QTextStream in(&f);
		QString qstr;
		std::string str;

		// Skip the first line.
		qstr = in.readLine();
		qstr = in.readLine();
		qstr.replace(",", " ");
		str = qstr.toStdString();
		std::istringstream iss(str);

		int imax;
		int jmax;
		int kmax;
		iss >> imax >> jmax >> kmax;

		// In case the grid is too big, import fails.
		if (imax * jmax > GridCreatingCondition::MAXGRIDSIZE){
			return false;
		}
		grid2d->setDimensions(imax, jmax);

		// Skip the third line.
		qstr = in.readLine();

		vtkPoints* points = vtkPoints::New();
		points->SetDataTypeToDouble();
		points->Allocate(imax * jmax);
		points->InsertPoint(imax * jmax - 1, 0, 0, 0);
		grid2d->vtkGrid()->SetPoints(points);

		// allocate memory for all grid related conditions.
		QList<GridAttributeContainer*>& clist = grid2d->gridAttributes();
		for (auto it = clist.begin(); it != clist.end(); ++it){
			(*it)->allocate();
		}
		for (int k = 0; k < kmax; ++k){
			for (int j = 0; j < jmax; ++j){
				for (int i = 0; i < imax; ++i){
					qstr = in.readLine();
					qstr.replace(",", " ");
					str = qstr.toStdString();
					std::istringstream iss(str);

					int dummyi, dummyj, dummyk;
					double x, y, z;

					iss >> dummyi >> dummyj >> dummyk >> x >> y >> z;

					if (k == 1){continue;}
					unsigned int id = grid2d->vertexIndex(i, j);
					points->InsertPoint(id, x, y, 0);
					container->setValue(id, z);
				}
			}
		}
	}
	f.close();
	return true;
}
