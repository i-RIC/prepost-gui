#include "riversurveydata.h"

#include <QStandardItem>

RiverSurveyData::RiverSurveyData(DataItem* parent) :
	DataItem {parent}
{}

RiverSurveyData::~RiverSurveyData()
{}

QStandardItem* RiverSurveyData::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("River Survey Data"));
	setupStandardItem(item);
	return item;
}
