#include "post3dwindownodevectorparticledataitem.h"
#include "post3dwindownodevectorparticlegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QStandardItem>

#include <vtkStructuredGrid.h>

Post3dWindowNodeVectorParticleDataItem::Post3dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_name (name)
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(this, SIGNAL(changed(Post3dWindowNodeVectorParticleDataItem*)),
					parent, SLOT(exclusivelyCheck(Post3dWindowNodeVectorParticleDataItem*)));
}

const std::string& Post3dWindowNodeVectorParticleDataItem::name() const
{
	return m_name;
}

void Post3dWindowNodeVectorParticleDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

void Post3dWindowNodeVectorParticleDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowNodeVectorParticleDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
