#ifndef TMSIMAGEDATAITEM_H
#define TMSIMAGEDATAITEM_H

#include "../named/namedgraphicwindowdataitem.h"

class TmsImageDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	TmsImageDataItem(const std::string name, const QString& caption, GraphicsWindowDataItem* parent);
	~TmsImageDataItem();
};

#endif // TMSIMAGEDATAITEM_H
