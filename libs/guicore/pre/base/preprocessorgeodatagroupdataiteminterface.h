#ifndef PREPROCESSORGEODATAGROUPDATAITEMINTERFACE_H
#define PREPROCESSORGEODATAGROUPDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"
#include "../../solverdef/solverdefinitiongridattribute.h"

#include <QIcon>

class Grid;
class GeoDataCreator;
class GeoDataPolygon;
class GeoDataRiverSurvey;
class PreProcessorGeoDataDataItemInterface;
class GridAttributeDimensionsContainer;
class GridAttributeEditWidget;
class GridAttributeStringConverter;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;

class GUICOREDLL_EXPORT PreProcessorGeoDataGroupDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorGeoDataGroupDataItemInterface(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent) :
		PreProcessorDataItem(cond->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent)
	{}
	virtual SolverDefinitionGridAttribute* condition() = 0;
	ProjectData* projectData() const {return ProjectDataItem::projectData();}

	virtual bool getValueRange(double* min, double* max) = 0;
	virtual void applyColorMapSetting() = 0;
	virtual void setupStringConverter(GridAttributeStringConverter* converter) = 0;
	virtual void setupEditWidget(GridAttributeEditWidget* widget) = 0;
	virtual void addCopyPolygon(GeoDataPolygon* polygon) = 0;
	virtual const QList<PreProcessorGeoDataDataItemInterface*> geoDatas() const = 0;
	virtual GridAttributeDimensionsContainer* dimensions() const = 0;
	virtual PreProcessorGeoDataDataItemInterface* buildGeoDataDataItem() = 0;
	virtual void addGeoData(PreProcessorGeoDataDataItemInterface *geoData) = 0;
	virtual GeoDataCreator* getPointMapCreator() = 0;

	// @todo ugly interface!
	virtual void openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection) = 0;
	virtual void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs) = 0;
	virtual void informCtrlPointUpdateToCrosssectionWindows() = 0;
	virtual void requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item) = 0;
	virtual void updateCrossectionWindows() = 0;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEMINTERFACE_H
