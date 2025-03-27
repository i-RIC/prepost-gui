#ifndef GEODATAPOLYLINEGROUPREADONLYCREATORT_DETAIL_H
#define GEODATAPOLYLINEGROUPREADONLYCREATORT_DETAIL_H

#include "../geodatapolylinegroupcellmappert.h"
#include "../geodatapolylinegroupcreator.h"
#include "../geodatapolylinegroupreadonlycreatort.h"
#include "../geodatapolylinegroupifacemappert.h"
#include "../geodatapolylinegroupjfacemappert.h"
#include "../geodatapolylinegroupnodemappert.h"

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>

#include <QStandardItem>

template <typename V, typename DA>
GeoDataPolyLineGroupReadOnlyCreatorT<V, DA>::GeoDataPolyLineGroupReadOnlyCreatorT(const QString& typeName) :
	GeoDataPolyLineGroupReadOnlyCreator {typeName}
{}

template <typename V, typename DA>
GeoDataPolyLineGroupReadOnlyCreatorT<V, DA>::~GeoDataPolyLineGroupReadOnlyCreatorT()
{}

template <typename V, typename DA>
GeoData* GeoDataPolyLineGroupReadOnlyCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto g = new GeoDataPolyLineGroup(parent, this, condition);
	PreProcessorGeoDataDataItemI* item = dynamic_cast<PreProcessorGeoDataDataItemI*>(parent);
	item->standardItem()->setData(QVariant(GeoDataPolyLineGroupCreator::tr("Deleting this item will also remove any graph windows associated with this data. Are you sure you want to delete this item?")), Qt::UserRole + 20);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		g->setMapper(new GeoDataPolyLineGroupNodeMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		g->setMapper(new GeoDataPolyLineGroupCellMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {
		g->setMapper(new GeoDataPolyLineGroupIFaceMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {
		g->setMapper(new GeoDataPolyLineGroupJFaceMapperT<V, DA>(this));
	}
	return g;
}

template <typename V, typename DA>
bool GeoDataPolyLineGroupReadOnlyCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return true;
}

#endif // GEODATAPOLYLINEGROUPREADONLYCREATORT_DETAIL_H
