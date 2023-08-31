#ifndef GEODATAPOLYLINEGROUPCREATORT_DETAIL_H
#define GEODATAPOLYLINEGROUPCREATORT_DETAIL_H

#include "../geodatapolylinegroupcellmappert.h"
#include "../geodatapolylinegroupcreatort.h"
#include "../geodatapolylinegroupnodemappert.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

#include <QStandardItem>

template <typename V, typename DA>
GeoDataPolyLineGroupCreatorT<V, DA>::GeoDataPolyLineGroupCreatorT(const QString& typeName) :
	GeoDataPolyLineGroupCreator {typeName}
{}

template <typename V, typename DA>
GeoDataPolyLineGroupCreatorT<V, DA>::~GeoDataPolyLineGroupCreatorT()
{}

template <typename V, typename DA>
GeoData* GeoDataPolyLineGroupCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto g = new GeoDataPolyLineGroup(parent, this, condition);
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent);
	item->standardItem()->setData(QVariant(GeoDataPolyLineGroupCreator::tr("Deleting this item will also remove any graph windows associated with this data. Are you sure you want to delete this item?")), Qt::UserRole + 20);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		g->setMapper(new GeoDataPolyLineGroupNodeMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		g->setMapper(new GeoDataPolyLineGroupCellMapperT<V, DA>(this));
	}
	return g;
}

template <typename V, typename DA>
bool GeoDataPolyLineGroupCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return true;
}

#endif // GEODATAPOLYLINEGROUPCREATORT_DETAIL_H
