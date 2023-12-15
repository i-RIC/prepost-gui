#ifndef PREPROCESSORGRIDCREATINGCONDITIONDATAITEMI_H
#define PREPROCESSORGRIDCREATINGCONDITIONDATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class GridCreatingCondition;
class PreProcessorGridAndGridCreatingConditionDataItemI;
class PreProcessorGridTypeDataItemI;

class QAction;

class GUICOREDLL_EXPORT PreProcessorGridCreatingConditionDataItemI : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorGridCreatingConditionDataItemI(GraphicsWindowDataItem* parent);
	virtual ~PreProcessorGridCreatingConditionDataItemI();

	virtual bool confirmOverwriteIfNeeded(QWidget* parent) = 0;
	virtual GridCreatingCondition* condition() const = 0;
	virtual void setCondition(GridCreatingCondition* condition) = 0;

	virtual PreProcessorGridTypeDataItemI* gridTypeDataItem() const = 0;
	virtual PreProcessorGridAndGridCreatingConditionDataItemI* gridAndGridCreatingConditionDataItem() const = 0;

	virtual QAction* createAction() const = 0;
	virtual QAction* showConditionAction() const = 0;
	virtual QAction* clearAction() const = 0;
	virtual QAction* switchAction() const = 0;
	virtual QAction* importAction() const = 0;
	virtual QAction* exportAction() const = 0;

	friend class GridCreatingCondition;
};

#endif // PREPROCESSORGRIDCREATINGCONDITIONDATAITEMINTERFACE
