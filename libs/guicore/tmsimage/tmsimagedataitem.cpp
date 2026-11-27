#include "tmsimagedataitem.h"

TmsImageDataItem::TmsImageDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedQStringGraphicWindowDataItem {name, caption, parent}
{
	setStandardItemToolTip(tr("Selects this internet map service as the background map source."));
}

TmsImageDataItem::~TmsImageDataItem()
{}
