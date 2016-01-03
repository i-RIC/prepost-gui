#include "geodatamapper.h"

GeoDataMapper::GeoDataMapper(const QString& caption, GeoDataCreator* parent) :
	QObject {parent},
	m_caption {caption},
	m_grid {nullptr},
	m_container {nullptr},
	m_geoData {nullptr}
{}

GeoDataMapper::~GeoDataMapper()
{}

const QString& GeoDataMapper::caption() const
{
	return m_caption;
}

void GeoDataMapper::setTarget(Grid* grid, GridAttributeContainer* container, GeoData* geodata)
{
	m_grid = grid;
	m_container = container;
	m_geoData = geodata;
}

GeoDataCreator* GeoDataMapper::creator() const
{
	return dynamic_cast<GeoDataCreator*>(parent());
}


Grid* GeoDataMapper::grid() const
{
	return m_grid;
}

GridAttributeContainer* GeoDataMapper::container() const
{
	return m_container;
}

GeoData* GeoDataMapper::geoData() const
{
	return m_geoData;
}
