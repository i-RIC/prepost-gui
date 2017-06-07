#ifndef LEFTBANKHWM_H
#define LEFTBANKHWM_H

#include "../points/points.h"

class LeftBankHWM : public Points
{
	Q_OBJECT

public:
	LeftBankHWM(DataItem* parent);
	~LeftBankHWM();

	QString caption() const override;

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // LEFTBANKHWM_H
