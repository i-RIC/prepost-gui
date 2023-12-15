#include "namedqstringgraphicwindowdataitem.h"

#include <QIcon>

NamedQStringGraphicWindowDataItem::NamedQStringGraphicWindowDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.svg"), parent),
	m_name (name)
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);
	connect(this, SIGNAL(changed(NamedQStringGraphicWindowDataItem*)), parent, SLOT(handleNamedItemChange(NamedQStringGraphicWindowDataItem*)));
}

NamedQStringGraphicWindowDataItem::~NamedQStringGraphicWindowDataItem()
{}

const  QString& NamedQStringGraphicWindowDataItem::name() const
{
	return m_name;
}

void NamedQStringGraphicWindowDataItem::handleStandardItemChange()
{
	emit changed(this);
}

void NamedQStringGraphicWindowDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void NamedQStringGraphicWindowDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
