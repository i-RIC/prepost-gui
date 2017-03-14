#ifndef RIVERSURVEYDATA_H
#define RIVERSURVEYDATA_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

class RiverSurveyData : public DataItem, public PreProcessorDataItemI
{
public:
	RiverSurveyData(DataItem* parent);
	~RiverSurveyData();

	QStandardItem* buildPreProcessorStandardItem() const override;
};

#endif // RIVERSURVEYDATA_H
