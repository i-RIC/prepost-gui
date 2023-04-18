#ifndef POST2DWINDOWGRIDTYPEDATAITEM_H
#define POST2DWINDOWGRIDTYPEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/misc/valuerangecontainer.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataiteminterface.h>

#include <map>
#include <unordered_map>
#include <vector>

class ColorMapSettingContainer;
class DelegatedColorMapSettingContainer;
class ModifyCommandDialog;
class SolverDefinitionGridType;
class Post2dWindowGeoDataTopDataItem;
class Post2dWindowZoneDataItem;
class PreProcessorGridTypeDataItemInterface;

class QAction;

class vtkActor2D;

class Post2dWindowGridTypeDataItem : public Post2dWindowDataItem, public Post2dWindowGridTypeDataItemInterface
{
	Q_OBJECT

public:
	Post2dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	~Post2dWindowGridTypeDataItem() override;

	const std::string& name() const;
	const std::vector<Post2dWindowZoneDataItem*>& zoneDatas() const;
	Post2dWindowZoneDataItem* zoneData(const std::string& name) const;

	SolverDefinitionGridType* gridType() const;
	Post2dWindowGeoDataTopDataItem* geoDataItem() const;
	ModifyCommandDialog* createApplyColorMapSettingDialog(const std::string& name, QWidget* parent) override;
	QUndoCommand* createApplyColorMapSettingCommand(const std::string& name, QUndoCommand* command, bool apply);

	const ValueRangeContainer& nodeValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& nodeValueRanges() const;
	const ValueRangeContainer& cellValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& cellValueRanges() const;
	const ValueRangeContainer& particleValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& particleValueRanges() const;
	const ValueRangeContainer& polyDataValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& polyDataValueRanges() const;
	DelegatedColorMapSettingContainer* colorMapSetting(const std::string& name) const;
	const std::unordered_map<std::string, DelegatedColorMapSettingContainer*>& colorMapSettingContainers() const;

	void setupZoneDataItems();
	void update();

	void handleResize(VTKGraphicsView* v) override;

public slots:
	void applyColorMapSetting(const std::string& name);
	void handlePreColorMapSettingUpdated(const std::string& name);

private:
	void setupColorMapSettingContainers(PreProcessorGridTypeDataItemInterface* item);

	void updateNodeValueRanges();
	void updateCellValueRanges();
	void updateParticleValueRanges();
	void updatePolyDataValueRanges();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	SolverDefinitionGridType* m_gridType;
	std::unordered_map<std::string, ValueRangeContainer> m_nodeValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_cellValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_particleValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_polyDataValueRanges;
	ValueRangeContainer m_dummyRange;

	std::unordered_map<std::string, DelegatedColorMapSettingContainer*> m_colorMapSettingContainers;
	std::unordered_map<std::string, vtkActor2D*> m_colorMapLegendActors;

	Post2dWindowGeoDataTopDataItem* m_geoDataItem;

	std::vector<Post2dWindowZoneDataItem*> m_zoneDatas;
	std::map<std::string, Post2dWindowZoneDataItem*> m_zoneDataNameMap;

	bool m_isZoneDataItemsSetup;

	class ApplyColorMapSettingCommand;
	class ApplyColorMapSettingDialog;
	class PreColorMapSettingUpdateHandler;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_H
