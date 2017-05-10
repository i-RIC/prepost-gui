#include "geodatamapper.h"
#include "private/geodatamapper_impl.h"

GeoDataMapper::GeoDataMapper(const QString& caption, GeoDataCreator* parent) :
	QObject {parent},
	impl {new Impl {}}
{
	impl->m_caption = caption;
	impl->m_grid = nullptr;
	impl->m_container = nullptr;
	impl->m_geoData = nullptr;
}

GeoDataMapper::~GeoDataMapper()
{
	delete impl;
}

const QString& GeoDataMapper::caption() const
{
	return impl->m_caption;
}

void GeoDataMapper::setTarget(Grid* grid, GridAttributeContainer* container, GeoData* geodata)
{
	impl->m_grid = grid;
	impl->m_container = container;
	impl->m_geoData = geodata;
}

GeoDataCreator* GeoDataMapper::creator() const
{
	return dynamic_cast<GeoDataCreator*>(parent());
}


Grid* GeoDataMapper::grid() const
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
