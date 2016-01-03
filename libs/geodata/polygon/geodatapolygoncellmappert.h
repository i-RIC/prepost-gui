#ifndef GEODATAPOLYGONCELLMAPPERT_H
#define GEODATAPOLYGONCELLMAPPERT_H

#include <guicore/pre/geodata/geodatacellmappert.h>
#include "geodatapolygon.h"
#include <vtkCell.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkSmartPointer.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolygonCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolygonCellMapperSetting() : GeoDataMapperSettingI() {}
	virtual ~GeoDataPolygonCellMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPolygonCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataPolygonCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapolygoncellmappert_detail.h"

#endif // GEODATAPOLYGONCELLMAPPERT_H
