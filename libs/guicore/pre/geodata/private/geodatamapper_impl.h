#ifndef GEODATAMAPPER_IMPL_H
#define GEODATAMAPPER_IMPL_H

#include "../geodatamapper.h"

class GeoDataMapper::Impl
{
public:
	QString m_caption;
	Grid* m_grid;
	GridAttributeContainer* m_container;
	GeoData* m_geoData;
};

#endif // GEODATAMAPPER_IMPL_H
