#ifndef DISTANCEMEASUREGROUPDATAITEM_IMPL_H
#define DISTANCEMEASUREGROUPDATAITEM_IMPL_H

#include "../distancemeasuregroupdataitem.h"

class QAction;

class DistanceMeasureGroupDataItem::Impl
{
public:
	Impl(DistanceMeasureGroupDataItem* parent);

	QAction* m_addAction;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;
};

#endif // DISTANCEMEASUREGROUPDATAITEM_IMPL_H
