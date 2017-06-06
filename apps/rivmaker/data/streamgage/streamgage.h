#ifndef STREAMGAGE_H
#define STREAMGAGE_H

#include "../points/points.h"

class StreamGage : public Points
{
public:
	StreamGage(DataItem* parent);
	~StreamGage();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // STREAMGAGE_H
