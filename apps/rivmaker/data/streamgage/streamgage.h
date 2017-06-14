#ifndef STREAMGAGE_H
#define STREAMGAGE_H

#include "../points/points.h"

class StreamGage : public Points
{
	Q_OBJECT

public:
	StreamGage(DataItem* parent);
	~StreamGage();

	QString caption() const override;

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;

private:
	QString relativeFilename() const override;
};

#endif // STREAMGAGE_H
