#ifndef POST2DWINDOWBCDATAITEM_IMPL_H
#define POST2DWINDOWBCDATAITEM_IMPL_H

#include "../post2dwindowbcdataitem.h"

class BoundaryConditionDialog;

class Post2dWindowBCDataItem::Impl
{
public:
	Impl(Post2dWindowBCDataItem* Item);
	~Impl();

	BoundaryConditionDialog* m_dialog;
	SolverDefinitionBoundaryCondition* m_condition;
};

#endif // POST2DWINDOWBCDATAITEM_IMPL_H
