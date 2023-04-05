#include "geodatamapper.h"
#include "geodatawithsinglemapper.h"

GeoDataWithSingleMapper::GeoDataWithSingleMapper(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData {d, creator, condition},
	m_mapper {nullptr}
{}

GeoDataWithSingleMapper::~GeoDataWithSingleMapper()
{
	delete m_mapper;
}

GeoDataMapper* GeoDataWithSingleMapper::mapper() const
{
	return m_mapper;
}

void GeoDataWithSingleMapper::setMapper(GeoDataMapper* mapper)
{
	m_mapper = mapper;
}
