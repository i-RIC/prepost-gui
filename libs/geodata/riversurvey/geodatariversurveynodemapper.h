#ifndef GEODATARIVERSURVEYNODEMAPPER_H
#define GEODATARIVERSURVEYNODEMAPPER_H

#include <guicore/pre/geodata/geodatanodemappert.h>

#include <vtkDoubleArray.h>

class GeoDataRiverSurveyNodeMapper : public GeoDataNodeMapperT<double, vtkDoubleArray>
{
public:
	GeoDataRiverSurveyNodeMapper(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATARIVERSURVEYNODEMAPPER_H
