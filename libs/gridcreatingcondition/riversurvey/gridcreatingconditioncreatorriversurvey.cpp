#include "gridcreatingconditioncreatorriversurvey.h"
#include "gridcreatingconditionriversurvey.h"

#include <guibase/irictoolbar.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataitemi.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <geodata/riversurvey/geodatariversurvey.h>

#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QMainWindow>

GridCreatingConditionCreatorRiverSurvey::GridCreatingConditionCreatorRiverSurvey() :
	GridCreatingConditionCreator()
{
	m_name = "riversurvey";
	m_caption = tr("Create grid from cross-section data");
	m_description = tr(
		"The grid shape is automatically defined using cross-section data. "
		"You can specify the grid division number, by adding grid creation "
		"control points on river crosssections, river center, left bank, and right bank.\n"
		"Note: This algorithm is available only when cross-section data is loaded!"
		"Please switch algorithm to this, after importing cross-section data.\n");
}

GridCreatingConditionCreatorRiverSurvey::~GridCreatingConditionCreatorRiverSurvey()
{}

SolverDefinitionGridType::GridType GridCreatingConditionCreatorRiverSurvey::gridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

GridCreatingCondition* GridCreatingConditionCreatorRiverSurvey::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionRiverSurvey(parent, this);
}

bool GridCreatingConditionCreatorRiverSurvey::checkAvailability(PreProcessorGridCreatingConditionDataItemI* condDataItem) const
{
	bool riverSurveyFound = false;
	PreProcessorGridTypeDataItemI* gtItem = dynamic_cast<PreProcessorGridTypeDataItemI*>(condDataItem->parent()->parent());
	PreProcessorGeoDataTopDataItemI* rtItem = gtItem->geoDataTop();
	for (PreProcessorGeoDataGroupDataItemI* gItem : rtItem->groupDataItems()) {
		QList<PreProcessorGeoDataDataItemI*> rItems = gItem->geoDatas();
		for (PreProcessorGeoDataDataItemI* rItem : rItems) {
			riverSurveyFound = riverSurveyFound || (dynamic_cast<GeoDataRiverSurvey*>(rItem->geoData()) != nullptr);
		}
	}
	if (! riverSurveyFound) {
		// No cross-section data exists. Not available.
		QWidget* w = condDataItem->mainWindow();
		QMessageBox::warning(w, tr("Warning"), tr("No cross-section data exists in geographic data now. This algorithmn is not available."));
		return false;
	}
	return true;
}
