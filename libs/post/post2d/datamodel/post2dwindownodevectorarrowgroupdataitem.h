#ifndef POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/arrows/arrowssettingcontainer.h>

#include <unordered_map>

class ArrowsSettingToolBarWidget;
class ColorMapSettingContainerI;
class NamedGraphicWindowDataItem;
class SolverDefinitionGridOutput;
class Post2dWindowZoneDataItem;
class Post2dWindowNodeVectorArrowGroupTopDataItem;
class ValueRangeContainer;

class Post2dWindowNodeVectorArrowGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowGroupDataItem(const std::string& target, Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorArrowGroupDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	std::string target() const;

	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void updateActorSetting() override;
	Post2dWindowNodeVectorArrowGroupTopDataItem* topDataItem() const;
	void createOrUpdateColorMapsSetting(SolverDefinitionGridOutput* output, const ValueRangeContainer& range);

	ArrowsSettingContainer m_setting;

	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;

	class UpdateActorSettingCommand;

private:
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void updateVisibility(bool visible) override;

	void innerUpdate2Ds() override;

	void setupActors();

	ColorMapSettingContainerI* colorMapSetting(const std::string& name) const;

	virtual vtkPointSet* buildFilteredData() = 0;

	vtkActor* m_actor;
	vtkActor2D* m_arrowLegendActor;
	vtkActor2D* m_colorLegendActor;

	ArrowsSettingToolBarWidget* m_arrowsToolBarWidget;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
