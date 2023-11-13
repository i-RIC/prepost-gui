#ifndef POST3DWINDOWSTRINGRESULTDATAITEM_H
#define POST3DWINDOWSTRINGRESULTDATAITEM_H

#include <postbase/string/poststringresultdataitem.h>

class Post3dWindowStringResultDataItem : public PostStringResultDataItem
{
public:
	Post3dWindowStringResultDataItem(GraphicsWindowDataItem* parent);

private:
	PostZoneDataItem* zoneDataItem() const override;
};

#endif // POST3DWINDOWSTRINGRESULTDATAITEM_H
