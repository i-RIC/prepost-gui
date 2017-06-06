#ifndef HUB_H
#define HUB_H

#include "../points/points.h"

class Hub : public Points
{
	Q_OBJECT

public:
	Hub(DataItem* parent);
	~Hub();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // HUB_H
