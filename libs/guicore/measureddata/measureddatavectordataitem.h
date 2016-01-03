#ifndef MEASUREDDATAVECTORDATAITEM_H
#define MEASUREDDATAVECTORDATAITEM_H

#include "../named/namedgraphicwindowdataitem.h"

class QAction;

class MeasuredDataVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	MeasuredDataVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
};

#endif // MEASUREDDATAVECTORDATAITEM_H
