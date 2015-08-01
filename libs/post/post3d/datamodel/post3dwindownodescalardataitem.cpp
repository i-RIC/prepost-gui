#include "post3dwindownodescalardataitem.h"

Post3dWindowNodeScalarDataItem::Post3dWindowNodeScalarDataItem(const QString& name, const QString& caption, Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_name {name}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(this, SIGNAL(changed(Post3dWindowNodeScalarDataItem*)),
					parent, SLOT(exclusivelyCheck(Post3dWindowNodeScalarDataItem*)));
}

void Post3dWindowNodeScalarDataItem::handleStandardItemChange()
{
	emit changed(this);
}
