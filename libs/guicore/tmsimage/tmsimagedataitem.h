#ifndef TMSIMAGEDATAITEM_H
#define TMSIMAGEDATAITEM_H

#include "../named/namedqstringgraphicwindowdataitem.h"

class TmsImageDataItem : public NamedQStringGraphicWindowDataItem
{
	Q_OBJECT

public:
	TmsImageDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent);
	~TmsImageDataItem();
};

#endif // TMSIMAGEDATAITEM_H
