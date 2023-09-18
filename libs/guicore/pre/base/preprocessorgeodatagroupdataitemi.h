#ifndef PREPROCESSORGEODATAGROUPDATAITEMI_H
#define PREPROCESSORGEODATAGROUPDATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"
#include "../../solverdef/solverdefinitiongridattribute.h"

#include <QIcon>

class GeoDataCreator;
class GeoDataPolygon;
class GeoDataRiverSurvey;
class PreProcessorGeoDataDataItemI;
class GridAttributeDimensionsContainer;
class GridAttributeEditWidget;
class GridAttributeStringConverter;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;

class GUICOREDLL_EXPORT PreProcessorGeoDataGroupDataItemI : public PreProcessorDataItem
{

public:
	PreProcessorGeoDataGroupDataItemI(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent);

	ProjectData* projectData() const;
	virtual SolverDefinitionGridAttribute* condition() = 0;
	virtual bool getValueRange(double* min, double* max) = 0;
	virtual void applyColorMapSetting() = 0;
	virtual void setupStringConverter(GridAttributeStringConverter* converter) = 0;
	virtual void setupEditWidget(GridAttributeEditWidget* widget) = 0;
	virtual void addCopyPolygon(GeoDataPolygon* polygon) = 0;
	virtual const QList<PreProcessorGeoDataDataItemI*> geoDatas() const = 0;
	virtual GridAttributeDimensionsContainer* dimensions() const = 0;
	virtual PreProcessorGeoDataDataItemI* buildGeoDataDataItem() = 0;
	virtual void addGeoData(PreProcessorGeoDataDataItemI *geoData) = 0;
	virtual GeoDataCreator* getPointMapCreator() = 0;

	// @todo ugly interface!
	virtual void openCrossSectionWindow(GeoDataRiverSurvey* rs, const QString& crosssection) = 0;
	virtual void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs) = 0;
	virtual void informCtrlPointUpdateToCrosssectionWindows() = 0;
	virtual void requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item) = 0;
	virtual void updateCrossectionWindows() = 0;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEMI_H
