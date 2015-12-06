#include "post2dbirdeyewindownodescalardataitem.h"

Post2dBirdEyeWindowNodeScalarDataItem::Post2dBirdEyeWindowNodeScalarDataItem(const std::string& name, const QString& caption, Post2dBirdEyeWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_name (name)
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(this, SIGNAL(changed(Post2dBirdEyeWindowNodeScalarDataItem*)),
					parent, SLOT(exclusivelyCheck(Post2dBirdEyeWindowNodeScalarDataItem*)));
}

void Post2dBirdEyeWindowNodeScalarDataItem::handleStandardItemChange()
{
	emit changed(this);
}

void Post2dBirdEyeWindowNodeScalarDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post2dBirdEyeWindowNodeScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
