#ifndef GEODATAPOINTGROUP_H
#define GEODATAPOINTGROUP_H

#include "gd_pointgroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroup.h>

#include <vector>

class GeoDataPoint;
class GeoDataPointGroupPoint;

class GD_POINTGROUP_EXPORT GeoDataPointGroup : public GeoDataPolyDataGroup
{
	Q_OBJECT

public:
	GeoDataPointGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPointGroup();

	std::vector<GeoDataPointGroupPoint*> pointsInBoundingBox(double xmin, double xmax, double ymin, double ymax) const;

	void setupMenu() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* d) override;

	GeoDataProxy* getProxy() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GeoDataPolyDataGroupPolyData* createNewData() override;
	GeoDataPolyData* createEditTargetData() override;
	void updateActorSetting() override;
	void updateMenu() override;
	void updateActorSettingForEditTargetPolyData() override;

	void updateVtkObjects() override;
	void updateSelectedDataVtkObjects() override;
	void selectDataWithPoint(const QPointF& point) override;
	void selectDataWithPolygon(const QPolygonF& polygon) override;
	std::vector<vtkActor*> actorsToShowWhenSelected() override;
	bool isMergablePolyData(GeoData* geoData) override;
	bool isMergablePolyDataGroup(GeoData* geoData) override;
	QString captionForData(int number) override;

	GeoDataPolyDataGroup* createInstanceForCopy(PreProcessorGeoDataDataItemInterface *d) override;

	class SetColorSettingCommand;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPointGroupCsvImporter;
	friend class GeoDataPointGroupProxy;
	friend class GeoDataPointGroupShpImporter;
};

#ifdef _DEBUG
	#include "private/geodatapointgroup_impl.h"
#endif // _DEBUG

#endif // GEODATAPOINTGROUP_H
