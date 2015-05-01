#include "gridcreatingconditioncreatorriversurvey.h"
#include "gridcreatingconditionriversurvey.h"

#include <guibase/irictoolbar.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>

#include <rawdata/riversurvey/rawdatariversurvey.h>

#include <QAction>
#include <QIcon>
#include <QMessageBox>

GridCreatingConditionCreatorRiverSurvey::GridCreatingConditionCreatorRiverSurvey()
	: GridCreatingConditionCreator()
{
	m_name = "riversurvey";
	m_caption = tr("Create grid from river survey data");
	m_description = tr(
		"The grid shape is automatically defined using river survey data. "
		"You can specify the grid division number, by adding grid creation "
		"control points on river crosssections, river center, left bank, and right bank.\n"
		"Note: This algorithm is available only when river survey data is loaded!"
		"Please switch algorithm to this, after importing river survey data.\n"
			);
}

GridCreatingCondition* GridCreatingConditionCreatorRiverSurvey::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionRiverSurvey(parent, this);
}

bool GridCreatingConditionCreatorRiverSurvey::checkAvailability(PreProcessorGridCreatingConditionDataItemInterface* condDataItem) const
{
	bool riverSurveyFound = false;
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(condDataItem->parent()->parent());
	PreProcessorRawDataTopDataItemInterface* rtItem = gtItem->rawdataTop();
	for (PreProcessorRawDataGroupDataItemInterface* gItem : rtItem->groupDataItems()) {
		QList<PreProcessorRawdataDataItemInterface*> rItems = gItem->rawDatas();
		for (PreProcessorRawdataDataItemInterface* rItem : rItems) {
			riverSurveyFound = riverSurveyFound || (dynamic_cast<RawDataRiverSurvey*>(rItem->rawData()) != nullptr);
		}
	}
	if (! riverSurveyFound) {
		// No river survey data exists. Not available.
		QWidget* w = condDataItem->mainWindow();
		QMessageBox::warning(w, tr("Warning"), tr("No river survey data exists in geographic data now. This algorithmn is not available."));
		return false;
	}
	return true;
}
