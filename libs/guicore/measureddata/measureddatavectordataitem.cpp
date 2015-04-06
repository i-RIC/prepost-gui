#include "measureddatavectordataitem.h"

#include <QStandardItem>

MeasuredDataVectorDataItem::MeasuredDataVectorDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem *parent)
	: GraphicsWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_name = name;
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	connect(this, SIGNAL(changed(MeasuredDataVectorDataItem*)),
			parent, SLOT(exclusivelyCheck(MeasuredDataVectorDataItem*)));
}

void MeasuredDataVectorDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}
