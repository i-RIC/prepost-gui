#include "riversurveydatadummy.h"

#include <QStandardItem>

RiverSurveyDataDummy::RiverSurveyDataDummy(DataItem* parent) :
	DataItem {parent}
{}

RiverSurveyDataDummy::~RiverSurveyDataDummy()
{}

QStandardItem* RiverSurveyDataDummy::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("River Survey Data (NO DATA)"));
	setupStandardItem(item);
	return item;
}
