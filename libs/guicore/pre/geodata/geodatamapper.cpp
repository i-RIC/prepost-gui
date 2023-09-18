#include "geodatamapper.h"
#include "private/geodatamapper_impl.h"

GeoDataMapper::GeoDataMapper(const QString& caption, QObject* parent) :
	QObject {parent},
	impl {new Impl {}}
{
	impl->m_caption = caption;
	impl->m_grid = nullptr;
	impl->m_container = nullptr;
	impl->m_geoData = nullptr;
}

GeoDataMapper::~GeoDataMapper()
{}

const QString& GeoDataMapper::caption() const
{
	return impl->m_caption;
}

void GeoDataMapper::setTarget(v4InputGrid* grid, GridAttributeContainer* container, GeoData* geodata)
{
	impl->m_grid = grid;
	impl->m_container = container;
	impl->m_geoData = geodata;
}

v4InputGrid* GeoDataMapper::grid() const
{
	return impl->m_grid;
}

GridAttributeContainer* GeoDataMapper::container() const
{
	return impl->m_container;
}

GeoData* GeoDataMapper::geoData() const
{
	return impl->m_geoData;
}
