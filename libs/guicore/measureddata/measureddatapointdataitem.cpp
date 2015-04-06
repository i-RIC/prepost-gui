#include "measureddatapointdataitem.h"

MeasuredDataPointDataItem::MeasuredDataPointDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem *parent)
	: GraphicsWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	m_name = name;

	connect(this, SIGNAL(changed(MeasuredDataPointDataItem*)),
			parent, SLOT(exclusivelyCheck(MeasuredDataPointDataItem*)));
}

void MeasuredDataPointDataItem::handleStandardItemChange()
{
	emit changed(this);
}
