#ifndef PREPROCESSORGRIDCREATINGCONDITIONDATAITEMINTERFACE_H
#define PREPROCESSORGRIDCREATINGCONDITIONDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class GridCreatingCondition;
class QAction;

class GUICOREDLL_EXPORT PreProcessorGridCreatingConditionDataItemInterface : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorGridCreatingConditionDataItemInterface(GraphicsWindowDataItem* parent);
	virtual ~PreProcessorGridCreatingConditionDataItemInterface();
	virtual GridCreatingCondition* condition() = 0;
	virtual void setCondition(GridCreatingCondition* condition) = 0;

	virtual QAction* createAction() = 0;
	virtual QAction* clearAction() = 0;
	virtual QAction* switchAction() = 0;

	friend class GridCreatingCondition;
};

#endif // PREPROCESSORGRIDCREATINGCONDITIONDATAITEMINTERFACE
