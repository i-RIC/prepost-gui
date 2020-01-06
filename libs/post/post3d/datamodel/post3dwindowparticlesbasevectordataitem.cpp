#include "post3dwindowparticlesbasevectordataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

Post3dWindowParticlesBaseVectorDataItem::Post3dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post3dWindowParticlesBaseVectorDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowParticlesBaseVectorDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
