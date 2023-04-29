#ifndef POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H
#define POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particledatasetting.h>

#include <unordered_map>

class ColorMapSettingContainer;
class ColorMapSettingToolBarWidget;
class NamedGraphicWindowDataItem;
class Post2dWindowGridTypeDataItem;
class Post2dWindowParticlesBaseScalarDataItem;
class Post2dWindowParticlesBaseTopDataItem;
class Post2dWindowZoneDataItem;

class QMenu;
class QMenuEvent;

class vtkActor;
class vtkActor2D;
class vtkPolyData;

class Post2dWindowParticlesBaseScalarGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowParticlesBaseScalarGroupDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowParticlesBaseScalarGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	ColorMapSettingContainer* colorMapSetting(const std::string& name) const;
	std::unordered_map<std::string, ColorMapSettingContainer*> colorMapSettings() const;

	void update();
	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void setupActors();
	void updateActorSetting() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	ColorMapSettingContainer* activeSetting() const;
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;
	Post2dWindowParticlesBaseScalarDataItem* activeChildDataItem() const;
	Post2dWindowParticlesBaseScalarDataItem* childDataItem(const std::string& name) const;

	vtkPolyData* particleData() const;
	void updateCheckState();

	vtkActor* m_actor;

	ParticleDataSetting m_setting;

	class ToolBarWidget;
	ToolBarWidget* m_toolBarWidget;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;

	class ToolBarWidgetController;
	ToolBarWidgetController* m_toolBarWidgetController;

	class UpdateActorSettingCommand;
	class PropertyDialog;

public:
	friend class Post2dWindowParticlesBaseScalarDataItem;
};

#endif // POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H
