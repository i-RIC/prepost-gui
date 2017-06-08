#include "rightbankhwm.h"
#include "rightbankhwmpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

RightBankHWM::RightBankHWM(DataItem* parent) :
	Points {parent}
{}

RightBankHWM::~RightBankHWM()
{}

QString RightBankHWM::caption() const
{
	return tr("Right Bank HWM");
}

QStandardItem* RightBankHWM::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconRBHWM.png"), tr("Right Bank HWM"));
	setupStandardItem(item);
	return item;
}

DataItemView* RightBankHWM::buildPreProcessorDataItemView(Model* model)
{
	return new RightBankHWMPreProcessorView(model, this);
}

QString RightBankHWM::relativeFilename() const
{
	return "rightbankhwm.dat";
}
