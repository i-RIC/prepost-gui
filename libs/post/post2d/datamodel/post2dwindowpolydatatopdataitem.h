#ifndef POST2DWINDOWPOLYDATATOPDATAITEM_H
#define POST2DWINDOWPOLYDATATOPDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowCalculationResultDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowPolyDataTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowPolyDataTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowPolyDataTopDataItem() override;

	void update();

	Post2dWindowCalculationResultDataItem* resultDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST2DWINDOWPOLYDATATOPDATAITEM_H
