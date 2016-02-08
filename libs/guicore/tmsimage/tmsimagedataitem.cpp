#include "tmsimagedataitem.h"

TmsImageDataItem::TmsImageDataItem(const std::string name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem {name, caption, parent}
{}

TmsImageDataItem::~TmsImageDataItem()
{}
