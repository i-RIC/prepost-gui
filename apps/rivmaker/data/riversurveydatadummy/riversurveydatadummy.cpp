#include "riversurveydatadummy.h"

#include <QIcon>
#include <QStandardItem>

RiverSurveyDataDummy::RiverSurveyDataDummy(DataItem* parent) :
	DataItem {parent}
{}

RiverSurveyDataDummy::~RiverSurveyDataDummy()
{}

QStandardItem* RiverSurveyDataDummy::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconRiverSurveyData.png"), tr("River Survey Data (NO DATA)"));
	setupStandardItem(item);
	return item;
}
