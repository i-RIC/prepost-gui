#ifndef GEODATAWITHSINGLEMAPPER_H
#define GEODATAWITHSINGLEMAPPER_H

#include "../../guicore_global.h"

#include "geodata.h"

class GUICOREDLL_EXPORT GeoDataWithSingleMapper : public GeoData
{
public:
	GeoDataWithSingleMapper(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataWithSingleMapper();

	GeoDataMapper* mapper() const override;
	void setMapper(GeoDataMapper* mapper);

private:
	GeoDataMapper* m_mapper;
};

#endif // GEODATAWITHSINGLEMAPPER_H
