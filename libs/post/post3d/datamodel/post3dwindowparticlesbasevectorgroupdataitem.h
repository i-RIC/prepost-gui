#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guibase/vtktool/vtkarrowlegendactors.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particledatavectorsetting.h>

class ArrowsSettingToolBarWidget;
class ColorMapSettingContainer;
class NamedGraphicWindowDataItem;
class Post3dWindowGridTypeDataItem;
class Post3dWindowParticlesBaseTopDataItem;

class vtkPolyData;
class vtkTransformFilter;

class Post3dWindowParticlesBaseVectorGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseVectorGroupDataItem(Post3dWindowDataItem* p);
	~Post3dWindowParticlesBaseVectorGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void update();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	QDialog* propertyDialog(QWidget* p) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showPropertyDialog() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void innerUpdate2Ds() override;
	void innerUpdateZScale(double zscale) override;

	void setupActors();
	void updateCheckState();
	void updateActorSetting() override;

	ColorMapSettingContainer* activeSetting() const;
	Post3dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post3dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	vtkPolyData* particleData() const;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	vtkTransformFilter* m_transformFilter;

	ParticleDataVectorSetting m_setting;
	double m_zScale;

	ArrowsSettingToolBarWidget* m_arrowsToolBarWidget;

	class SettingEditWidget;
	class UpdateSettingCommand;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
