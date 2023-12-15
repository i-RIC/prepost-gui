#ifndef PREPROCESSORGRIDCREATINGCONDITIONDATAITEMINTERFACE_H
#define PREPROCESSORGRIDCREATINGCONDITIONDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class GridCreatingCondition;
class PreProcessorGridTypeDataItemInterface;

class QAction;

class GUICOREDLL_EXPORT PreProcessorGridCreatingConditionDataItemInterface : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorGridCreatingConditionDataItemInterface(GraphicsWindowDataItem* parent);
	virtual ~PreProcessorGridCreatingConditionDataItemInterface();

	virtual GridCreatingCondition* condition() const = 0;
	virtual void setCondition(GridCreatingCondition* condition) = 0;
	virtual bool confirmOverwriteIfNeeded(QWidget* parent) = 0;

	virtual PreProcessorGridTypeDataItemInterface* gridTypeDataItem() const = 0;

	virtual QAction* createAction() const = 0;
	virtual QAction* showConditionAction() const = 0;
	virtual QAction* clearAction() const = 0;
	virtual QAction* switchAction() const = 0;
	virtual QAction* importAction() const = 0;
	virtual QAction* exportAction() const = 0;

	friend class GridCreatingCondition;
};

#endif // PREPROCESSORGRIDCREATINGCONDITIONDATAITEMINTERFACE
