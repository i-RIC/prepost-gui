#ifndef REFERENCEMARK_H
#define REFERENCEMARK_H

#include "../points/points.h"

class ReferenceMark : public Points
{
	Q_OBJECT

public:
	ReferenceMark(DataItem* parent);
	~ReferenceMark();

	QString caption() const override;

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;

private:
	QString relativeFilename() const override;
};

#endif // REFERENCEMARK_H
