#ifndef GEODATAPOLYGONGROUP_H
#define GEODATAPOLYGONGROUP_H

#include "gd_polygongroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroup.h>

#include <vector>

class GeoDataPolygon;
class GeoDataPolygonGroupPolygon;

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroup : public GeoDataPolyDataGroup
{
	Q_OBJECT

public:
	GeoDataPolygonGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPolygonGroup();

	std::vector<GeoDataPolygonGroupPolygon*> polygonsInBoundingBox(double xmin, double xmax, double ymin, double ymax) const;

	void setupMenu() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void showPropertyDialog() override;

	GeoDataProxy* getProxy() override;

	class DisplaySettingWidget;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GeoDataPolyDataGroupPolyData* createNewData() override;
	GeoDataPolyData* createEditTargetData() override;
	void updateActorSetting() override;
	void updateActionConnections();
	void updateMenu() override;
	void updateMenu(QMenu* menu);
	void updateActorSettingForEditTargetPolyData() override;

	void updateVtkObjects() override;
	void updateSelectedDataVtkObjects() override;
	void selectDataWithPoint(const QPointF& point) override;
	void selectDataWithPolygon(const QPolygonF& polygon) override;
	std::vector<vtkActor*> actorsToShowWhenSelected() override;
	bool isMergablePolyData(GeoData* geoData) override;
	bool isMergablePolyDataGroup(GeoData* geoData) override;
	QString captionForData(int number) override;

	GeoDataPolyDataGroup* createInstanceForCopy(PreProcessorGeoDataDataItemI* d) override;

	class DisplaySetting;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPolygonGroupAttributeBrowser;
	friend class GeoDataPolygonGroupCsvImporter;
	friend class GeoDataPolygonGroupProxy;
	friend class GeoDataPolygonGroupShpImporter;
	friend class GeoDataPointmap;
};

#endif // GEODATAPOLYGONGROUP_H
