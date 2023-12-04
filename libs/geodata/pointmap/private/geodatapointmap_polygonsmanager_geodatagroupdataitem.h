#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_GEODATAGROUPDATAITEM_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_GEODATAGROUPDATAITEM_H

#include "geodatapointmap_polygonsmanager.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>

class GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem : public PreProcessorGeoDataGroupDataItemI
{
public:
	GeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, GeoDataTopDataItem* topDataItem, PreProcessorDataItem* parent);
	~GeoDataGroupDataItem();

	GeoDataDataItem* geoDataDataItem() const;

	ProjectData* projectData() const override;
	SolverDefinitionGridAttribute* condition() override;
	bool getValueRange(double* min, double* max) override;
	void applyColorMapSetting() override;
	void setupStringConverter(GridAttributeStringConverter* converter) override;
	void setupEditWidget(GridAttributeEditWidget* widget) override;
	void addCopyPolygon(GeoDataPolygon* polygon) override;
	const QList<PreProcessorGeoDataDataItemI*> geoDatas() const override;
	GridAttributeDimensionsContainer* dimensions() const override;
	PreProcessorGeoDataDataItemI* buildGeoDataDataItem() override;
	void addGeoData(PreProcessorGeoDataDataItemI *geoData) override;
	GeoDataCreator* getPointMapCreator() override;

	void openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection) override;
	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs) override;
	void informCtrlPointUpdateToCrosssectionWindows() override;
	void requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item) override;
	void updateCrossectionWindows() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	SolverDefinitionGridAttribute* m_condition;
	GeoDataDataItem* m_geoDataDataItem;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_GEODATAGROUPDATAITEM_H
