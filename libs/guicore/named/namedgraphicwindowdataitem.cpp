#include "namedgraphicwindowdataitem.h"

#include <QIcon>

NamedGraphicWindowDataItem::NamedGraphicWindowDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent),
	m_name (name)
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);
	connect(this, SIGNAL(changed(NamedGraphicWindowDataItem*)), parent, SLOT(handleNamedItemChange(NamedGraphicWindowDataItem*)));
}

NamedGraphicWindowDataItem::~NamedGraphicWindowDataItem()
{}

const std::string& NamedGraphicWindowDataItem::name() const
{
	return m_name;
}

void NamedGraphicWindowDataItem::handleStandardItemChange()
{
	emit changed(this);
}

void NamedGraphicWindowDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void NamedGraphicWindowDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
