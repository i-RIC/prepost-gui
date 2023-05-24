#ifndef POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/arrows/arrowssettingcontainer.h>
#include <guicore/misc/targeted/targeteditemi.h>

#include <unordered_map>

class ArrowsSettingToolBarWidget;
class ColorMapSettingContainer;
class NamedGraphicWindowDataItem;
class Post2dWindowZoneDataItem;
class ValueRangeContainer;

class Post2dWindowNodeVectorArrowGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowGroupDataItem(Post2dWindowDataItem* parent);
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

	void update();

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void updateActorSetting() override;
	void updateCheckState();
	Post2dWindowZoneDataItem* zoneDataItem() const;
	void createOrUpdateColorMapsSetting(const std::string& name, const QString& caption, const ValueRangeContainer& range);

	ArrowsSettingContainer m_setting;

	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;

	class UpdateActorSettingCommand;

private:
	std::string target() const override;
	void setTarget(const std::string& target) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	void innerUpdate2Ds() override;

	void setupActors();

	ColorMapSettingContainer* colorMapSetting(const std::string& name) const;

	virtual vtkPointSet* buildFilteredData() = 0;

	vtkActor* m_actor;
	vtkActor2D* m_arrowLegendActor;
	vtkActor2D* m_colorLegendActor;

	ArrowsSettingToolBarWidget* m_arrowsToolBarWidget;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
