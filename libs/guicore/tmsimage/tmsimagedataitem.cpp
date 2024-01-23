#include "tmsimagedataitem.h"

TmsImageDataItem::TmsImageDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedQStringGraphicWindowDataItem {name, caption, parent}
{}

TmsImageDataItem::~TmsImageDataItem()
{}
