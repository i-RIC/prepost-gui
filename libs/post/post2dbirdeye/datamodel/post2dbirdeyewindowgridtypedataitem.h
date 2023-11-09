#ifndef POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H
#define POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

#include <misc/valuerangecontainer.h>

#include <map>
#include <unordered_map>
#include <vector>

class SolverDefinitionGridType;
class Post2dBirdEyeWindowZoneDataItem;

class QAction;

class Post2dBirdEyeWindowGridTypeDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	~Post2dBirdEyeWindowGridTypeDataItem();

	const std::string& name() const;
	const std::vector<Post2dBirdEyeWindowZoneDataItem*>& zoneDatas() const;
	Post2dBirdEyeWindowZoneDataItem* zoneData(const std::string& name) const;

	SolverDefinitionGridType* gridType() const;

	const ValueRangeContainer& nodeValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& nodeValueRanges() const;
	const ValueRangeContainer& cellValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& cellValueRanges() const;
	const ValueRangeContainer& iEdgeValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& iEdgeValueRanges() const;
	const ValueRangeContainer& jEdgeValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& JEdgeValueRanges() const;

	void setupZoneDataItems();
	void update();

private:
	void updateNodeValueRanges();
	void updateCellValueRanges();
	void updateIEdgeValueRanges();
	void updateJEdgeValueRanges();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	SolverDefinitionGridType* m_gridType;
	std::unordered_map<std::string, ValueRangeContainer> m_nodeValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_cellValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_iEdgeValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_jEdgeValueRanges;
	ValueRangeContainer m_dummyRange;

	std::vector<Post2dBirdEyeWindowZoneDataItem*> m_zoneDatas;
	std::map<std::string, Post2dBirdEyeWindowZoneDataItem*> m_zoneDataNameMap;

	bool m_isZoneDataItemsSetup;
};

#endif // POST2DBIRDEYEWINDOWGRIDTYPEDATAITEM_H
