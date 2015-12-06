#include "post3dwindownodevectorstreamlinedataitem.h"
#include "post3dwindownodevectorstreamlinegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QStandardItem>

#include <vtkStructuredGrid.h>

Post3dWindowNodeVectorStreamlineDataItem::Post3dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_name (name)
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(this, SIGNAL(changed(Post3dWindowNodeVectorStreamlineDataItem*)), parent, SLOT(exclusivelyCheck(Post3dWindowNodeVectorStreamlineDataItem*)));
}

const std::string& Post3dWindowNodeVectorStreamlineDataItem::name() const
{
	return m_name;
}

void Post3dWindowNodeVectorStreamlineDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

void Post3dWindowNodeVectorStreamlineDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowNodeVectorStreamlineDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
