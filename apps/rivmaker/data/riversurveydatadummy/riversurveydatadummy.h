#ifndef RIVERSURVEYDATADUMMY_H
#define RIVERSURVEYDATADUMMY_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

class RiverSurveyDataDummy : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	RiverSurveyDataDummy(DataItem* dataItem);
	~RiverSurveyDataDummy();

	QStandardItem* buildPreProcessorStandardItem() const override;
};

#endif // RIVERSURVEYDATADUMMY_H
