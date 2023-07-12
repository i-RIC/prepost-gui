#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/vtktool/vtkarrowlegendactors.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particledatavectorsetting.h>

class ArrowsSettingToolBarWidget;
class ColorMapSettingContainerI;
class NamedGraphicWindowDataItem;
class Post2dWindowGridTypeDataItem;
class Post2dWindowParticlesBaseTopDataItem;

class Post2dWindowParticlesBaseVectorGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowParticlesBaseVectorGroupDataItem(Post2dWindowDataItem* p);
	~Post2dWindowParticlesBaseVectorGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	QDialog* propertyDialog(QWidget* p) override;
	void handleStandardItemChange() override;

	ColorMapSettingContainerI* activeColorMapSetting() const;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showPropertyDialog() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void innerUpdate2Ds() override;

	void setupActors();
	void updateCheckState();
	void updateActorSetting() override;

	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;
	vtkPolyData* particleData() const;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	ParticleDataVectorSetting m_setting;

	ArrowsSettingToolBarWidget* m_arrowsToolBarWidget;

	class SettingEditWidget;
	class UpdateSettingCommand;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
