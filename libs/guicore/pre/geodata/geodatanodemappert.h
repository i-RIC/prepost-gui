#ifndef GEODATANODEMAPPERT_H
#define GEODATANODEMAPPERT_H

#include "geodatamappert.h"
#include "../gridcond/base/gridattributenodecontainert.h"

template <class V, class DA>
class GeoDataNodeMapperT : public GeoDataMapperT<V>
{
public:
	GeoDataNodeMapperT(const QString& caption, GeoDataCreator* parent);
	virtual ~GeoDataNodeMapperT();

protected:
	GridAttributeNodeContainerT<V, DA>* container() const;
};

#include "private/geodatanodemappert_detail.h"

#endif // GEODATANODEMAPPERT_H
