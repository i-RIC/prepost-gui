#include "post3dwindownodevectorstreamlinedataitem.h"
#include "post3dwindownodevectorstreamlinegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QStandardItem>

#include <vtkStructuredGrid.h>

Post3dWindowNodeVectorStreamlineDataItem::Post3dWindowNodeVectorStreamlineDataItem(const QString& name, const QString& caption, Post3dWindowDataItem* parent)
	: Post3dWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_name = name;
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	connect(this, SIGNAL(changed(Post3dWindowNodeVectorStreamlineDataItem*)),
			parent, SLOT(exclusivelyCheck(Post3dWindowNodeVectorStreamlineDataItem*)));

}

void Post3dWindowNodeVectorStreamlineDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

