#ifndef PREPROCESSORDATAITEMI_H
#define PREPROCESSORDATAITEMI_H

class DataItemController;
class DataItemView;
class Model;

class QStandardItem;

class PreProcessorDataItemI
{
public:
	virtual ~PreProcessorDataItemI() {}

	virtual QStandardItem* buildPreProcessorStandardItem() const {return nullptr;}
	virtual DataItemController* buildPreProcessorDataItemController() {return nullptr;}
	virtual DataItemView* buildPreProcessorDataItemView(Model*) {return nullptr;}
};

#endif // PREPROCESSORDATAITEMI_H
