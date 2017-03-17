#include "leftbankhwm.h"
#include "leftbankhwmpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

LeftBankHWM::LeftBankHWM(DataItem* parent) :
	Points {parent}
{}

LeftBankHWM::~LeftBankHWM()
{}

QStandardItem* LeftBankHWM::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconLBHWM.png"), tr("Left Bank HWM"));
	setupStandardItem(item);
	return item;
}

DataItemView* LeftBankHWM::buildPreProcessorDataItemView(Model* model)
{
	return new LeftBankHWMPreProcessorView(model, this);
}
