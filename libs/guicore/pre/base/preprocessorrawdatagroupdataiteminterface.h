#ifndef PREPROCESSORRAWDATAGROUPDATAITEMINTERFACE_H
#define PREPROCESSORRAWDATAGROUPDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"
#include "preprocessorrawdatadataiteminterface.h"
#include "../../solverdef/solverdefinitiongridattribute.h"

class Grid;
class QStandardItem;
class RawDataPolygon;
class RawDataRiverSurvey;
class GridAttributeDimensionsContainer;
class GridAttributeEditWidget;
class RawDataRiverSurveyCrosssectionWindowProjectDataItem;

class GUICOREDLL_EXPORT PreProcessorRawDataGroupDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorRawDataGroupDataItemInterface(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent)
		: PreProcessorDataItem(cond->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
	{}
	virtual SolverDefinitionGridAttribute* condition() = 0;
	virtual bool getValueRange(double* min, double* max) = 0;
	virtual void setupEditWidget(GridAttributeEditWidget* widget = nullptr) = 0;
	virtual void addCopyPolygon(RawDataPolygon* polygon) = 0;
	virtual const QList<PreProcessorRawdataDataItemInterface*> rawDatas() const = 0;
	virtual GridAttributeDimensionsContainer* dimensions() const = 0;

	// @todo ugly interface!
	virtual void openCrossSectionWindow(RawDataRiverSurvey* rs, double crosssection) = 0;
	virtual void toggleCrosssectionWindowsGridCreatingMode(bool gridMode, RawDataRiverSurvey* rs) = 0;
	virtual void informCtrlPointUpdateToCrosssectionWindows() = 0;
	virtual void requestCrosssectionWindowDelete(RawDataRiverSurveyCrosssectionWindowProjectDataItem* item) = 0;
	virtual void updateCrossectionWindows() = 0;
};

#endif // PREPROCESSORRAWDATAGROUPDATAITEMINTERFACE_H
