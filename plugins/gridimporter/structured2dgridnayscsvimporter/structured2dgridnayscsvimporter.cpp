#include "structured2dgridnayscsvimporter.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainert.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <misc/stringtool.h>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QVector2D>
#include <QString>
#include <QMessageBox>
#include <sstream>

namespace {

struct AttributeData {
	enum ValueType {Real, Int};
	enum Position {Node, Cell};

	bool valid = true;
	ValueType valueType;
	Position position;

	GridAttributeContainerT<double>* realContainer = nullptr;
	GridAttributeContainerT<int>* intContainer = nullptr;
};


} // namespace

Structured2DGridNaysCSVImporter::Structured2DGridNaysCSVImporter() :
	GridImporterInterface(), QObject()
{}

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

		qstr = in.readLine();
		QStringList headerFrags = qstr.split(",");
		for (int i = 0; i < 6; ++i) {
			headerFrags.pop_front();
		}
		std::vector<AttributeData> attData;
		QStringList ignoredCols;
		for (int i = 0; i < headerFrags.size(); ++i) {
			QString n = headerFrags.at(i);
			QStringRef name(&n, 2, n.length() - 2);
			QStringRef typeFlag(&n, 0, 1);

			AttributeData data;
			auto att = grid2d->gridAttribute(iRIC::toStr(name.toString()));
			if (att == nullptr) {
				data.valid = false;
				attData.push_back(data);
				ignoredCols.push_back(n);
				continue;
			}

			if (att->gridAttribute()->position() == SolverDefinitionGridAttribute::Node && typeFlag == "N") {
				data.position = AttributeData::Node;
			} else if (att->gridAttribute()->position() == SolverDefinitionGridAttribute::CellCenter && typeFlag == "C") {
				data.position = AttributeData::Cell;
			} else {
				data.valid = false;
				attData.push_back(data);
				continue;
			}
			auto realAtt = dynamic_cast<GridAttributeContainerT<double>* > (att);
			auto intAtt = dynamic_cast<GridAttributeContainerT<int>* > (att);
			if (realAtt != nullptr) {
				data.valueType = AttributeData::Real;
				data.realContainer = realAtt;
			} else if (intAtt != nullptr) {
				data.valueType = AttributeData::Int;
				data.intContainer = intAtt;
			}
			attData.push_back(data);
		}

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

					for (const AttributeData& data : attData) {
						if (! data.valid) {
							// ignore this column
							double dummy;
							iss >> dummy;
							continue;
						}
						if (data.position == AttributeData::Cell && (i == imax - 1 || j == jmax - 1)) {continue;}
						if (data.position == AttributeData::Node) {
							id = grid2d->vertexIndex(i, j);
						} else {
							id = grid2d->cellIndex(i, j);
						}
						if (data.valueType == AttributeData::Real) {
							double v;
							iss >> v;
							data.realContainer->setValue(id, v);
						} else {
							int v;
							iss >> v;
							data.intContainer->setValue(id, v);
						}
					}
				}
			}
		}
		if (ignoredCols.size() > 0) {
			QMessageBox::information(parent, tr("Information"), tr("The following columns were ignored.\n%1").arg(ignoredCols.join(", ")));
		}
	}
	f.close();
	return true;
}
