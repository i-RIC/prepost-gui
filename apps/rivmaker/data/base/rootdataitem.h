#ifndef ROOTDATAITEM_H
#define ROOTDATAITEM_H

#include "dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

class RootDataItem : public DataItem, public PreProcessorDataItemI
{
public:
	RootDataItem(Project* project);
	~RootDataItem();

	Project* project() const override;

	QStandardItem* buildPreProcessorStandardItem() const override;
	virtual DataItemView* buildPreProcessorDataItemView(Model* model) override;

private:
	Project* m_project;
};

#endif // ROOTDATAITEM_H
