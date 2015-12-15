#include "post3dwindowparticlesvectordataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

Post3dWindowParticlesVectorDataItem::Post3dWindowParticlesVectorDataItem(const QString& name, const QString& caption, Post3dWindowDataItem* parent)
	: Post3dWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_name = name;
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	connect(this, SIGNAL(changed(Post3dWindowParticlesVectorDataItem*)),
					parent, SLOT(exclusivelyCheck(Post3dWindowParticlesVectorDataItem*)));
}

void Post3dWindowParticlesVectorDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}
