#include "structured2dgridnayscsvexporter.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QVariant>
#include <QVector2D>

namespace {

struct AttributeData
{
	enum ValueType {Real, Int};

	QString name;
	ValueType valueType;
	std::vector<double> realValues;
	std::vector<int> intValues;
};

template <typename V, typename DA>
std::vector<V> buildNodeAttributeValues(GridAttributeContainerT<V, DA>* c)
{
	std::vector<V> ret;
	ret.reserve(c->dataCount());
	for (unsigned int i = 0; i < c->dataCount(); ++i) {
		ret.push_back(c->value(i));
	}
	return ret;
}

template <typename V, typename DA>
std::vector<V> buildCellAttributeValues(GridAttributeContainerT<V, DA>* c)
{
	std::vector<V> ret;
	ret.reserve(c->dataCount());
	auto grid = dynamic_cast<v4Structured2dGrid*> (c->grid()->grid());
	for (int j = 0; j < grid->dimensionJ() - 1; ++j) {
		for (int i = 0; i < grid->dimensionI() - 1; ++i) {
			ret.push_back(c->value(grid->cellIndex(i, j)));
		}
		ret.push_back(0);
	}
	for (unsigned int i = 0; i < grid->dimensionI(); ++i) {
		ret.push_back(0);
	}
	return ret;
}


AttributeData buildNodeAttributeValues(GridAttributeContainer* c)
{
	AttributeData ret;
	ret.name = QString("N_%1").arg(c->name().c_str());
	auto realC = dynamic_cast<GridAttributeContainerT<double, vtkDoubleArray>* > (c);
	auto intC = dynamic_cast<GridAttributeContainerT<int, vtkIntArray>* > (c);
	if (realC != nullptr) {
		ret.valueType = AttributeData::Real;
		ret.realValues =  buildNodeAttributeValues(realC);
	} else {
		ret.valueType = AttributeData::Int;
		ret.intValues = buildNodeAttributeValues(intC);
	}
	return ret;
}

AttributeData buildCellAttributeValues(GridAttributeContainer* c)
{
	AttributeData ret;
	ret.name = QString("C_%1").arg(c->name().c_str());
	auto realC = dynamic_cast<GridAttributeContainerT<double, vtkDoubleArray>* > (c);
	auto intC = dynamic_cast<GridAttributeContainerT<int, vtkIntArray>* > (c);
	if (realC != nullptr) {
		ret.valueType = AttributeData::Real;
		ret.realValues = buildCellAttributeValues(realC);
	} else {
		ret.valueType = AttributeData::Int;
		ret.intValues = buildCellAttributeValues(intC);
	}
	return ret;
}

} // namespace

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

bool Structured2DGridNaysCSVExporter::doExport(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	auto grid2d = dynamic_cast<v4Structured2dGrid*> (grid->grid());
	int imax = grid2d->dimensionI();
	int jmax = grid2d->dimensionJ();
	int kmax = 1;

	QFile f(filename);
	if (! f.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text)){return false;}

	std::vector<AttributeData> attributeData;

	for (auto att : grid->attributes()) {
		if (att->gridAttribute()->position() == SolverDefinitionGridAttribute::Position::Node) {
			attributeData.push_back(buildNodeAttributeValues(att));
		} else if (att->gridAttribute()->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
			attributeData.push_back(buildCellAttributeValues(att));
		}
	}

	QTextStream o(&f);
	o.setRealNumberPrecision(8);
	o << "IMAX,JMAX,KMAX" << endl;
	o << imax << "," << jmax << "," << kmax << endl;

	o << "I,J,K,X,Y,Z";
	for (const auto& data : attributeData) {
		o << "," << data.name;
	}
	o << endl;
	int idx = 0;
	for (int k = 0; k < kmax; k++){
		for (int j = 0; j < jmax; j++){
			for (int i = 0; i < imax; i++){
				o << scientific;
				o << i << "," << j << "," << k << ","
					<< grid2d->point2d(i, j).x() << ","
					<< grid2d->point2d(i, j).y() << ","
					<< 0;
				for (const AttributeData& data : attributeData) {
					if (data.valueType == AttributeData::Real) {
						o << "," << data.realValues.at(idx);
					} else {
						o << "," << data.intValues.at(idx);
					}
				}
				o << endl;
				++ idx;
			}
		}
	}
	f.close();
	return true;
}
