#ifndef REFERENCEMARK_H
#define REFERENCEMARK_H

#include "../points/points.h"

class ReferenceMark : public Points
{
public:
	ReferenceMark(DataItem* parent);
	~ReferenceMark();

	QString caption() const override;

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // REFERENCEMARK_H
