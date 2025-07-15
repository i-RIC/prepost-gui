#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/post/postzonedataitem.h>

#include <h5cgnszone.h>

#include <memory>

class Post2dWindowCalculationResultDataItem;
class Post2dWindowGridTypeDataItem;
class Post2dWindowInputGridDataItem;
class v4PostZoneDataContainer;

class Post2dWindowZoneDataItem : public Post2dWindowDataItem, public PostZoneDataItem
{
	Q_OBJECT

public:
	Post2dWindowZoneDataItem(const std::string& zoneName, Post2dWindowDataItem* parent);
	~Post2dWindowZoneDataItem();

	void addCustomMenuItems(QMenu* menu) override;

	void updateZDepthRangeItemCount() override;

	v4PostZoneDataContainer* v4DataContainer() override;
	std::string zoneName() const;
	void update(bool noParticle = false);
	void setEdgeFocus(vtkIdType i, vtkIdType j);
	void setEdgeFocus(const std::vector<vtkIdType>& line);
	void clearEdgeFocus();

	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowInputGridDataItem* inputGridDataItem() const;
	Post2dWindowCalculationResultDataItem* resultDataItem() const;

	void updateRegionPolyData();

private:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doViewOperationEndedGlobal(VTKGraphicsView* v) override;

	void setupActors();

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // POST2DWINDOWZONEDATAITEM_H
