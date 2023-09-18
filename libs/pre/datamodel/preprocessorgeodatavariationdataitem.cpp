#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatavariationdataitem.h"

PreProcessorGeoDataVariationDataItem::PreProcessorGeoDataVariationDataItem(PreProcessorGeoDataGroupDataItem* item) :
	PreProcessorGeoDataGroupDataItemI {
		dynamic_cast<PreProcessorGeoDataGroupDataItem*>(item)->condition(),item}
{}

PreProcessorGeoDataVariationDataItem::~PreProcessorGeoDataVariationDataItem()
{}

SolverDefinitionGridAttribute* PreProcessorGeoDataVariationDataItem::condition()
{
	return groupDataItem()->condition();
}

bool PreProcessorGeoDataVariationDataItem::getValueRange(double* min, double* max)
{
	return true;
}

void PreProcessorGeoDataVariationDataItem::applyColorMapSetting()
{

}

void PreProcessorGeoDataVariationDataItem::setupStringConverter(GridAttributeStringConverter* converter)
{
	groupDataItem()->setupStringConverter(converter);
}

void PreProcessorGeoDataVariationDataItem::setupEditWidget(GridAttributeEditWidget* widget)
{
	groupDataItem()->setupEditWidget(widget);
}

void PreProcessorGeoDataVariationDataItem::addCopyPolygon(GeoDataPolygon* polygon)
{

}

const QList<PreProcessorGeoDataDataItemI*> PreProcessorGeoDataVariationDataItem::geoDatas() const
{
	QList<PreProcessorGeoDataDataItemI*> ret;

	return ret;
}

GridAttributeDimensionsContainer* PreProcessorGeoDataVariationDataItem::dimensions() const
{
	return groupDataItem()->dimensions();
}

PreProcessorGeoDataDataItemI* PreProcessorGeoDataVariationDataItem::buildGeoDataDataItem()
{
	return groupDataItem()->buildGeoDataDataItem();
}

void PreProcessorGeoDataVariationDataItem::addGeoData(PreProcessorGeoDataDataItemI *geoData)
{

}

GeoDataCreator* PreProcessorGeoDataVariationDataItem::getPointMapCreator()
{
	return groupDataItem()->getPointMapCreator();
}

void PreProcessorGeoDataVariationDataItem::openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection)
{
	return groupDataItem()->openCrossSectionWindow(rs, crosssection);
}

void PreProcessorGeoDataVariationDataItem::toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs)
{

}

void PreProcessorGeoDataVariationDataItem::informCtrlPointUpdateToCrosssectionWindows()
{

}

void PreProcessorGeoDataVariationDataItem::requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item)
{

}

void PreProcessorGeoDataVariationDataItem::updateCrossectionWindows()
{

}


PreProcessorGeoDataGroupDataItem* PreProcessorGeoDataVariationDataItem::groupDataItem() const
{
	return dynamic_cast<PreProcessorGeoDataGroupDataItem*> (parent());
}
