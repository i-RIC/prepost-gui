#include <QStandardItem>
#include "post2dwindowgraphdataitem.h"

Post2dWindowGraphDataItem::Post2dWindowGraphDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{
	m_standardItem->setToolTip(tr("Selects the result used for the graph display."));
	if (m_standardItemCopy != nullptr) {m_standardItemCopy->setToolTip(m_standardItem->toolTip());}
}

Post2dWindowGraphDataItem::~Post2dWindowGraphDataItem()
{}
