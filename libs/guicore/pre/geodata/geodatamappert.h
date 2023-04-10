#ifndef GEODATAMAPPERT_H
#define GEODATAMAPPERT_H

#include "geodatamapper.h"
#include "../gridcond/base/gridattributecontainert.h"

template <class V>
class GeoDataMapperT : public GeoDataMapper
{
public:
	GeoDataMapperT(const QString& caption, QObject* parent);

protected:
	GridAttributeContainerT<V>* container() const;
	V fromVariant(const QVariant& val) const;
};

#include "private/geodatamappert_detail.h"

#endif // GEODATAMAPPERT_H
