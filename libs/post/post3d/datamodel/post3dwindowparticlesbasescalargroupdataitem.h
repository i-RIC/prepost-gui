#ifndef POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particledatasetting.h>

#include <unordered_map>

class ColorMapSettingContainerI;
class ColorMapSettingToolBarWidget;
class NamedGraphicWindowDataItem;
class Post3dWindowGridTypeDataItem;
class Post3dWindowParticlesBaseScalarDataItem;
class Post3dWindowParticlesBaseTopDataItem;
class Post3dWindowZoneDataItem;

class QMenu;
class QMenuEvent;

class vtkActor;
class vtkActor2D;
class vtkPolyData;

class Post3dWindowParticlesBaseScalarGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseScalarGroupDataItem(Post3dWindowDataItem* parent);
	virtual ~Post3dWindowParticlesBaseScalarGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	ColorMapSettingContainerI* colorMapSetting(const std::string& name) const;
	std::unordered_map<std::string, ColorMapSettingContainerI*> colorMapSettings() const;
	ColorMapSettingContainerI* activeColorMapSetting() const;

	void update();
	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void setupActors();
	void updateActorSetting() override;

	void innerUpdateZScale(double zscale) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	Post3dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post3dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	Post3dWindowParticlesBaseScalarDataItem* activeChildDataItem() const;
	Post3dWindowParticlesBaseScalarDataItem* childDataItem(const std::string& name) const;

	vtkPolyData* particleData() const;
	void updateCheckState();

	vtkActor* m_actor;

	ParticleDataSetting m_setting;

	class ToolBarWidget;
	ToolBarWidget* m_toolBarWidget;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;

	class ToolBarWidgetController;
	ToolBarWidgetController* m_toolBarWidgetController;

	class SettingEditWidget;

public:
	friend class Post3dWindowParticlesBaseScalarDataItem;
};

#endif // POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H
