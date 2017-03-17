#ifndef ARBITRARYHWM_H
#define ARBITRARYHWM_H

#include "../points/points.h"

class ArbitraryHWM : public Points
{
	Q_OBJECT

public:
	ArbitraryHWM(DataItem* parent);
	~ArbitraryHWM();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // ARBITRARYHWM_H
