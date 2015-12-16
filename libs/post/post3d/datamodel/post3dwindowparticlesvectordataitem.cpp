#include "post3dwindowparticlesvectordataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

Post3dWindowParticlesVectorDataItem::Post3dWindowParticlesVectorDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post3dWindowParticlesVectorDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowParticlesVectorDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
