#include <QStandardItem>
#include "tmsimagedataitem.h"

TmsImageDataItem::TmsImageDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedQStringGraphicWindowDataItem {name, caption, parent}
{
	if (m_standardItem != nullptr) {
		m_standardItem->setToolTip(tr("Selects this internet map service as the background map source."));
		if (m_standardItemCopy != nullptr) {m_standardItemCopy->setToolTip(m_standardItem->toolTip());}
	}
}

TmsImageDataItem::~TmsImageDataItem()
{}
