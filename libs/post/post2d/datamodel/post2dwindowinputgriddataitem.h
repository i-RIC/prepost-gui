#ifndef POST2DWINDOWINPUTGRIDDATAITEM_H
#define POST2DWINDOWINPUTGRIDDATAITEM_H

#include "post2dwindowgriddataitemi.h"

#include <memory>

class v4InputGrid;
class Post2dWindowBCGroupDataItem;
class Post2dWindowGridAttributeNodeGroupDataItem;
class Post2dWindowGridAttributeCellGroupDataItem;
class Post2dWindowGridAttributeIEdgeGroupDataItem;
class Post2dWindowGridAttributeJEdgeGroupDataItem;
class Post2dWindowGridShapeDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowInputGridDataItem : public Post2dWindowGridDataItemI
{
	Q_OBJECT

public:
	Post2dWindowInputGridDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowInputGridDataItem();

	v4InputGrid* inputGrid() const;
	v4Grid2d* grid() const override;

	Post2dWindowZoneDataItem* zoneDataItem() const;
	Post2dWindowGridShapeDataItem* gridShapeDataItem() const;
	Post2dWindowGridAttributeNodeGroupDataItem* nodeGroupDataItem() const;
	Post2dWindowGridAttributeCellGroupDataItem* cellGroupDataItem() const;
	Post2dWindowGridAttributeIEdgeGroupDataItem* iEdgeGroupDataItem() const;
	Post2dWindowGridAttributeJEdgeGroupDataItem* jEdgeGroupDataItem() const;
	Post2dWindowBCGroupDataItem* bcGroupDataItem() const;

	void update();
	bool colorBarShouldBeVisible(const std::string& name) const;
	void assignActorZValues(const ZDepthRange& range) override;

private slots:
	void handleGridChange();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // POST2DWINDOWINPUTGRIDDATAITEM_H
