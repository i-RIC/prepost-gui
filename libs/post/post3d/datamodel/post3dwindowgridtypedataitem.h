#ifndef POST3DWINDOWGRIDTYPEDATAITEM_H
#define POST3DWINDOWGRIDTYPEDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guicore/misc/valuerangecontainer.h>

#include <map>
#include <unordered_map>
#include <vector>

class SolverDefinitionGridType;
class Post3dWindowZoneDataItem;

class QAction;

class vtkActor2D;

class Post3dWindowGridTypeDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	~Post3dWindowGridTypeDataItem();

	const std::string& name() const;
	const std::vector<Post3dWindowZoneDataItem*>& zoneDatas() const;
	Post3dWindowZoneDataItem* zoneData(const std::string& name) const;

	SolverDefinitionGridType* gridType() const;
	bool isChildCaptionAvailable(const QString& caption);

	const ValueRangeContainer& nodeValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& nodeValueRanges() const;
	const ValueRangeContainer& cellValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& cellValueRanges() const;
	const ValueRangeContainer& particleValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& particleValueRanges() const;

	void setupZoneDataItems();
	void update();

private:
	void updateNodeValueRanges();
	void updateCellValueRanges();
	void updateParticleValueRanges();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	SolverDefinitionGridType* m_gridType;
	std::unordered_map<std::string, ValueRangeContainer> m_nodeValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_cellValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_particleValueRanges;
	ValueRangeContainer m_dummyRange;

	std::vector<Post3dWindowZoneDataItem*> m_zoneDatas;
	std::map<std::string, Post3dWindowZoneDataItem*> m_zoneDataNameMap;

	bool m_isZoneDataItemsSetup;
};

#endif // POST3DWINDOWGRIDTYPEDATAITEM_H
