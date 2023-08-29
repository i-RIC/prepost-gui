#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guibase/vtktool/vtkarrowlegendactors.h>
#include <postbase/particle/particledatavectorsetting.h>

class ArrowsSettingToolBarWidget;
class ColorMapSettingContainerI;
class NamedGraphicWindowDataItem;
class Post3dWindowGridTypeDataItem;
class Post3dWindowParticlesBaseTopDataItem;
class Post3dWindowParticlesBaseVectorGroupTopDataItem;

class vtkPolyData;
class vtkTransformFilter;

class Post3dWindowParticlesBaseVectorGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseVectorGroupDataItem(const std::string& target, Post3dWindowDataItem* p);
	~Post3dWindowParticlesBaseVectorGroupDataItem();

	std::string target() const;

	void update();
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	QDialog* propertyDialog(QWidget* p) override;
	void handleStandardItemChange() override;

	ColorMapSettingContainerI* activeColorMapSetting() const;
	ColorMapSettingContainerI* activeColorMapSettingWithVisibleLegend() const;

public slots:
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
	void updateVisibility(bool visible) override;

	Post3dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	Post3dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post3dWindowParticlesBaseVectorGroupTopDataItem* vectorTopDataItem() const;
	vtkPolyData* particleData() const;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	ParticleDataVectorSetting m_setting;
	double m_zScale;

	ArrowsSettingToolBarWidget* m_arrowsToolBarWidget;

	class SettingEditWidget;
	class UpdateSettingCommand;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
