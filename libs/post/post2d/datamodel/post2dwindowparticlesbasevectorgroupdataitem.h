#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/vtktool/vtkarrowlegendactors.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particledatavectorsetting.h>

class ColorMapSettingContainer;
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
	void handleResize(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* p) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdate2Ds() override;

	void setupActors();
	void updateCheckState();
	void updateActorSettings();

	ColorMapSettingContainer* activeSetting() const;
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;
	vtkPolyData* particleData() const;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	ParticleDataVectorSetting m_setting;

	class PropertyDialog;
	class UpdateSettingCommand;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_H
