#include "post3dwindowcontourgrouptopdataitem.h"

#include "post3dwindowcontourgroupdataitem.h"


Post3dWindowContourGroupTopDataItem::Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p) : 
	Post3dWindowDataItem {tr("Contours"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

Post3dWindowContourGroupTopDataItem::~Post3dWindowContourGroupTopDataItem()
{
}

void Post3dWindowContourGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto item  = new Post3dWindowContourGroupDataItem(this);
	item->updateZScale(m_zScale);
	item->loadFromProjectMainFile(node);
	m_childItems.push_back(item);
}

void Post3dWindowContourGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		(*it)->saveToProjectMainFile(writer);
	}
}

void Post3dWindowContourGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowContourGroupDataItem*>(item);
		child->update();
	}
}

void Post3dWindowContourGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}
