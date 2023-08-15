#ifndef MEASUREDDATAFILEDATAITEM_IMPL_H
#define MEASUREDDATAFILEDATAITEM_IMPL_H

#include "../measureddatafiledataitem.h"

class QAction;

class MeasuredDataFileDataItem::Impl
{
public:
	Impl(MeasuredData* md);

	MeasuredData* m_measuredData;

	MeasuredDataPointGroupDataItem* m_pointGroupDataItem;
	MeasuredDataVectorGroupTopDataItem* m_vectorGroupDataItem;

	QAction* m_exportAction;
};

#endif // MEASUREDDATAFILEDATAITEM_IMPL_H
