#ifndef PREPROCESSORGEODATAVARIATIONDATAITEM_H
#define PREPROCESSORGEODATAVARIATIONDATAITEM_H

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>

class PreProcessorGeoDataGroupDataItem;

class PreProcessorGeoDataVariationDataItem : public PreProcessorGeoDataGroupDataItemInterface
{
public:
	PreProcessorGeoDataVariationDataItem(PreProcessorGeoDataGroupDataItem* item);
	~PreProcessorGeoDataVariationDataItem() override;

	SolverDefinitionGridAttribute* condition() override;
	bool getValueRange(double* min, double* max) override;
	void applyColorMapSetting() override;
	void setupStringConverter(GridAttributeStringConverter* converter) override;
	void setupEditWidget(GridAttributeEditWidget* widget) override;
	void addCopyPolygon(GeoDataPolygon* polygon) override;
	const QList<PreProcessorGeoDataDataItemInterface*> geoDatas() const override;
	GridAttributeDimensionsContainer* dimensions() const override;
	PreProcessorGeoDataDataItemInterface* buildGeoDataDataItem() override;
	void addGeoData(PreProcessorGeoDataDataItemInterface *geoData) override;
	GeoDataCreator* getPointMapCreator() override;

	void openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection) override;
	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs) override;
	void informCtrlPointUpdateToCrosssectionWindows() override;
	void requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item) override;
	void updateCrossectionWindows() override;

private:
	PreProcessorGeoDataGroupDataItem* groupDataItem() const;
};

#endif // PREPROCESSORGEODATAVARIATIONDATAITEM_H
