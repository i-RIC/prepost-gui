#ifndef GEODATAPOLYLINEGROUP_H
#define GEODATAPOLYLINEGROUP_H

#include "gd_polylinegroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroup.h>

#include <vector>

class GeoDataPolyLine;
class GeoDataPolyLineGroupPolyLine;

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroup : public GeoDataPolyDataGroup
{
	Q_OBJECT

public:
	GeoDataPolyLineGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPolyLineGroup();

	std::vector<GeoDataPolyLineGroupPolyLine*> polyLinesInBoundingBox(double xmin, double xmax, double ymin, double ymax) const;

	void setupMenu() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

private:
	GeoDataPolyDataGroupPolyData* createNewData() override;
	GeoDataPolyData* createEditTargetData() override;
	void updateActorSetting() override;
	void updateMenu() override;

	void updateVtkObjects() override;
	void updateSelectedDataVtkObjects() override;
	void selectDataWithPoint(const QPointF& point) override;
	void selectDataWithPolygon(const QPolygonF& polygon) override;
	std::vector<vtkActor*> actorsToShowWhenSelected() override;
	bool isMergablePolyData(GeoData* geoData) override;
	bool isMergablePolyDataGroup(GeoData* geoData) override;
	QString captionForData(int number) override;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPolyLineGroupAttributeBrowser;
	friend class GeoDataPolyLineGroupShpImporter;
};

#endif // GEODATAPOLYLINEGROUP_H
