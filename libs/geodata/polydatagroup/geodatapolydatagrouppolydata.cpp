#include "geodatapolydatagrouppolydata.h"
#include "private/geodatapolydatagrouppolydata_impl.h"

GeoDataPolyDataGroupPolyData::Impl::Impl(GeoDataPolyDataGroup* group) :
	m_id {},
	m_name {},
	m_value {},
	m_order {0},
	m_indexOffset {0},
	m_group {group}
{}

GeoDataPolyDataGroupPolyData::GeoDataPolyDataGroupPolyData(GeoDataPolyDataGroup* group) :
	impl {new Impl {group}}
{}

GeoDataPolyDataGroupPolyData::~GeoDataPolyDataGroupPolyData()
{
	delete impl;
}

GeoDataPolyDataGroup* GeoDataPolyDataGroupPolyData::group() const
{
	return impl->m_group;
}

QString GeoDataPolyDataGroupPolyData::id() const
{
	return impl->m_id;
}

void GeoDataPolyDataGroupPolyData::setId(const QString& id)
{
	impl->m_id = id;
}

QString GeoDataPolyDataGroupPolyData::name() const
{
	return impl->m_name;
}

void GeoDataPolyDataGroupPolyData::setName(const QString& name)
{
	impl->m_name = name;
}

QVariant GeoDataPolyDataGroupPolyData::value() const
{
	return impl->m_value;
}

void GeoDataPolyDataGroupPolyData::setValue(const QVariant& v)
{
	impl->m_value = v;
}

unsigned int GeoDataPolyDataGroupPolyData::order() const
{
	return impl->m_order;
}

void GeoDataPolyDataGroupPolyData::setOrder(unsigned int order)
{
	impl->m_order = order;
}

unsigned int GeoDataPolyDataGroupPolyData::indexOffset() const
{
	return impl->m_indexOffset;
}

void GeoDataPolyDataGroupPolyData::setIndexOffset(unsigned int offset)
{
	impl->m_indexOffset = offset;
}
