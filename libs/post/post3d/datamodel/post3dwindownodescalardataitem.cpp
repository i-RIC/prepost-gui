#include "post3dwindownodescalardataitem.h"

Post3dWindowNodeScalarDataItem::Post3dWindowNodeScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post3dWindowNodeScalarDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowNodeScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
