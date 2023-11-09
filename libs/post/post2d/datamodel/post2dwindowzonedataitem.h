#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/post/postzonedataitem.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>

#include <h5cgnszone.h>

class QAction;
class QSignalMapper;

class Post2dWindowCalculationResultDataItem;
class Post2dWindowGridTypeDataItem;
class Post2dWindowInputGridDataItem;
class PostZoneDataContainer;
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
	bool isMasked() const;
	std::string zoneName() const;
	void update(bool noParticle = false);

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

	Post2dWindowInputGridDataItem* m_inputGridDataItem;
	Post2dWindowCalculationResultDataItem* m_resultDataItem;

	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	std::string m_zoneName;
	bool m_isMasked;
};

#endif // POST2DWINDOWZONEDATAITEM_H
