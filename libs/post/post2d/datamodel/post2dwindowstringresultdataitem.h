#ifndef POST2DWINDOWSTRINGRESULTDATAITEM_H
#define POST2DWINDOWSTRINGRESULTDATAITEM_H

#include <postbase/string/poststringresultdataitem.h>

class Post2dWindowStringResultDataItem : public PostStringResultDataItem
{
public:
	Post2dWindowStringResultDataItem(GraphicsWindowDataItem* parent);

private:
	PostZoneDataItem* zoneDataItem() const override;
};

#endif // POST2DWINDOWSTRINGRESULTDATAITEM_H
