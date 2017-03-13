#ifndef PREPROCESSORDATAITEMI_H
#define PREPROCESSORDATAITEMI_H

class DataItemController;
class DataItemView;

class QStandardItem;

class PreProcessorDataItemI
{
public:
	virtual ~PreProcessorDataItemI() {}

	virtual QStandardItem* buildPreProcessorStandardItem() const = 0;
	virtual DataItemController* buildPreProcessorDataItemController() const = 0;
	virtual DataItemView* buildPreProcessorDataItemView() const = 0;
};

#endif // PREPROCESSORDATAITEMI_H
