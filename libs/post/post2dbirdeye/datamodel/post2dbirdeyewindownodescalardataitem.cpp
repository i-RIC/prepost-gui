#include "post2dbirdeyewindownodescalardataitem.h"

Post2dBirdEyeWindowNodeScalarDataItem::Post2dBirdEyeWindowNodeScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dBirdEyeWindowNodeScalarDataItem::~Post2dBirdEyeWindowNodeScalarDataItem()
{}

void Post2dBirdEyeWindowNodeScalarDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post2dBirdEyeWindowNodeScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
