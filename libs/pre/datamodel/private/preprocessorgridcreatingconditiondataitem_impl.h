#ifndef PREPROCESSORGRIDCREATINGCONDITIONDATAITEM_IMPL_H
#define PREPROCESSORGRIDCREATINGCONDITIONDATAITEM_IMPL_H

#include "../preprocessorgridcreatingconditiondataitem.h"

class PreProcessorGridCreatingConditionDataItem::Impl
{
public:
	GridCreatingCondition* m_condition;

	QAction* m_createAction;
	QAction* m_switchAlgorithmAction;
	QAction* m_deleteAction;
	QAction* m_clearAction;
};

#endif // PREPROCESSORGRIDCREATINGCONDITIONDATAITEM_IMPL_H
