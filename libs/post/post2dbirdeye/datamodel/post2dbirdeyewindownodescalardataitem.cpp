#include "post2dbirdeyewindownodescalardataitem.h"

Post2dBirdEyeWindowNodeScalarDataItem::Post2dBirdEyeWindowNodeScalarDataItem(const QString& name, const QString& caption, Post2dBirdEyeWindowDataItem* parent)
	: Post2dBirdEyeWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	m_name = name;

	connect(this, SIGNAL(changed(Post2dBirdEyeWindowNodeScalarDataItem*)),
					parent, SLOT(exclusivelyCheck(Post2dBirdEyeWindowNodeScalarDataItem*)));
}

void Post2dBirdEyeWindowNodeScalarDataItem::handleStandardItemChange()
{
	emit changed(this);
}
