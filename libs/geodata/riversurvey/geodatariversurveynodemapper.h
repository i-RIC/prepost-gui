#ifndef GEODATARIVERSURVEYNODEMAPPER_H
#define GEODATARIVERSURVEYNODEMAPPER_H

#include <guicore/pre/geodata/geodatanodemappert.h>
#include "geodatariversurvey.h"
#include <vtkCell.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <misc/doublemappingsetting.h>

class GeoDataRiverSurveyNodeMapper : public GeoDataNodeMapperT<double, vtkDoubleArray>
{

public:
	GeoDataRiverSurveyNodeMapper(GeoDataCreator* parent)
		: GeoDataNodeMapperT<double, vtkDoubleArray>(parent) {
		GeoDataNodeMapperT<double, vtkDoubleArray>::m_caption = "River survey node mapper";
	}
	GeoDataMapperSetting* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSetting* s) override;
	void terminate(GeoDataMapperSetting* s) override;
};

#endif // GEODATARIVERSURVEYNODEMAPPER_H
