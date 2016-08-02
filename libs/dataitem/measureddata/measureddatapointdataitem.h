#ifndef MEASUREDDATAPOINTDATAITEM_H
#define MEASUREDDATAPOINTDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class MeasuredDataPointDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	MeasuredDataPointDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
};

#endif // MEASUREDDATAPOINTDATAITEM_H
