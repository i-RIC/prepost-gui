#ifndef POST2DWINDOWGRAPHDATAITEM_H
#define POST2DWINDOWGRAPHDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowGraphDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGraphDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowGraphDataItem();
};

#endif // POST2DWINDOWGRAPHDATAITEM_H
