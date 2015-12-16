#include "post3dwindownodevectorparticledataitem.h"

Post3dWindowNodeVectorParticleDataItem::Post3dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post3dWindowNodeVectorParticleDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowNodeVectorParticleDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
