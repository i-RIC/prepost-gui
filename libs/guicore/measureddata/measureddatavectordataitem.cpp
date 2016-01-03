#include "measureddatavectordataitem.h"

#include <QStandardItem>

MeasuredDataVectorDataItem::MeasuredDataVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem {name, caption, parent}
{}
