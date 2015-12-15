#include "post3dwindowparticlesscalardataitem.h"

Post3dWindowParticlesScalarDataItem::Post3dWindowParticlesScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post3dWindowParticlesScalarDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowParticlesScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
