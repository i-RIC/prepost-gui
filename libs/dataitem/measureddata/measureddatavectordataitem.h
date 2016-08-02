#ifndef MEASUREDDATAVECTORDATAITEM_H
#define MEASUREDDATAVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class MeasuredDataVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	MeasuredDataVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
};

#endif // MEASUREDDATAVECTORDATAITEM_H
