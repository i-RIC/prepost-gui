#include "arbitraryhwm.h"
#include "arbitraryhwmpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

ArbitraryHWM::ArbitraryHWM(DataItem* parent) :
	Points {parent}
{}

ArbitraryHWM::~ArbitraryHWM()
{}

QStandardItem* ArbitraryHWM::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconAHWM.png"), tr("Arbitrary  HWM"));
	setupStandardItem(item);
	return item;
}

DataItemView* ArbitraryHWM::buildPreProcessorDataItemView(Model* model)
{
	return new ArbitraryHWMPreProcessorView(model, this);
}
