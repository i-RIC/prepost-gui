#ifndef GEODATAPOLYDATAGROUPPOLYDATA_IMPL_H
#define GEODATAPOLYDATAGROUPPOLYDATA_IMPL_H

#include <QVariant>

#include "../geodatapolydatagrouppolydata.h"

class GeoDataPolyDataGroupPolyData::Impl
{
public:
	Impl(GeoDataPolyDataGroup* group);

	QString m_id;
	QString m_name;
	QVariant m_value;

	unsigned int m_order;
	unsigned int m_indexOffset;

	GeoDataPolyDataGroup* m_group;
};

#endif // GEODATAPOLYDATAGROUPPOLYDATA_IMPL_H
