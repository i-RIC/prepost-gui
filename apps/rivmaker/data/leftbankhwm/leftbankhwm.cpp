#include "leftbankhwm.h"
#include "leftbankhwmpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

LeftBankHWM::LeftBankHWM(DataItem* parent) :
	Points {parent}
{}

LeftBankHWM::~LeftBankHWM()
{}

QString LeftBankHWM::caption() const
{
	return tr("Left Bank HWM");
}

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
