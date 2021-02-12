#include "gridcreatingconditioncreatorriversurvey15d.h"
#include "gridcreatingconditionriversurvey15d.h"

#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <geodata/riversurvey/geodatariversurvey.h>

#include <QMessageBox>
#include <QMainWindow>

GridCreatingConditionCreatorRiverSurvey15D::GridCreatingConditionCreatorRiverSurvey15D() :
	GridCreatingConditionCreator()
{
	m_name = "riversurvey15d";
	m_caption = tr("Create 1.5D grid from cross-section data");
	m_description = tr(
		"The grid shape is automatically defined using cross-section data. "
		"You can specify the grid division number, by adding grid creation "
		"control points on river center.\n"
		"Note: This algorithm is available only when cross-section data is loaded!"
		"Please switch algorithm to this, after importing cross-section data.\n");
}

GridCreatingConditionCreatorRiverSurvey15D::~GridCreatingConditionCreatorRiverSurvey15D()
{}

SolverDefinitionGridType::GridType GridCreatingConditionCreatorRiverSurvey15D::gridType() const
{
	return SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection;
}

GridCreatingCondition* GridCreatingConditionCreatorRiverSurvey15D::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionRiverSurvey15D(parent, this);
}

bool GridCreatingConditionCreatorRiverSurvey15D::checkAvailability(PreProcessorGridCreatingConditionDataItemInterface* condDataItem) const
{
	bool riverSurveyFound = false;
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(condDataItem->parent()->parent());
	PreProcessorGeoDataTopDataItemInterface* rtItem = gtItem->geoDataTop();
	for (PreProcessorGeoDataGroupDataItemInterface* gItem : rtItem->groupDataItems()) {
		QList<PreProcessorGeoDataDataItemInterface*> rItems = gItem->geoDatas();
		for (PreProcessorGeoDataDataItemInterface* rItem : rItems) {
			riverSurveyFound = riverSurveyFound || (dynamic_cast<GeoDataRiverSurvey*>(rItem->geoData()) != nullptr);
		}
	}
	if (! riverSurveyFound) {
		// No cross-section data exists. Not available.
		QWidget* w = condDataItem->mainWindow();
		QMessageBox::warning(w, tr("Warning"), tr("No cross-section data exists in geographic data now. This algorithm is not available."));
		return false;
	}
	return true;
}
