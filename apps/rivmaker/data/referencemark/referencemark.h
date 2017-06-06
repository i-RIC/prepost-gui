#ifndef REFERENCEMARK_H
#define REFERENCEMARK_H

#include "../points/points.h"

class ReferenceMark : public Points
{
public:
	ReferenceMark(DataItem* parent);
	~ReferenceMark();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // REFERENCEMARK_H
