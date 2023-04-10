#ifndef GEODATAPOLYLINECREATORT_DETAIL_H
#define GEODATAPOLYLINECREATORT_DETAIL_H

#include "../geodatapolylinecreatort.h"
#include "../geodatapolylinecellmappert.h"
#include "../geodatapolylinenodemappert.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

#include <QStandardItem>

template <class V, class DA>
GeoDataPolyLineCreatorT<V, DA>::GeoDataPolyLineCreatorT(const QString& typeName) :
	GeoDataPolyLineCreator {typeName}
{}

template <class V, class DA>
GeoData* GeoDataPolyLineCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataPolyLine* polyline = new GeoDataPolyLine(parent, this, condition);
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent);
	item->standardItem()->setData(QVariant("Deleting this item will also remove any graph windows associated with this polyline.  Are you sure you want to delete this item?"), Qt::UserRole + 20);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		polyline->setMapper(new GeoDataPolyLineNodeMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		polyline->setMapper(new GeoDataPolyLineCellMapperT<int, vtkIntArray>(this));
	}
	return polyline;
}

template <class V, class DA>
bool GeoDataPolyLineCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return condition->isReferenceInformation();
}

#endif // GEODATAPOLYLINECREATORT_DETAIL_H
