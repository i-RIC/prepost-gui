#ifndef POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/particle/particledatasetting.h>

#include <unordered_map>

class ColorMapSettingContainer;
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

	ColorMapSettingContainer* colorMapSetting(const std::string& name) const;
	std::unordered_map<std::string, ColorMapSettingContainer*> colorMapSettings() const;

	void update();
	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void handleResize(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void setupActors();
	void updateActorSettings();

	void innerUpdateZScale(double zscale) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	ColorMapSettingContainer* activeSetting() const;
	Post3dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post3dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	Post3dWindowParticlesBaseScalarDataItem* activeChildDataItem() const;
	Post3dWindowParticlesBaseScalarDataItem* childDataItem(const std::string& name) const;

	vtkPolyData* particleData() const;
	void updateCheckState();

	vtkActor* m_actor;

	ParticleDataSetting m_setting;

	class UpdateActorSettingsCommand;
	class PropertyDialog;

public:
	friend class Post3dWindowParticlesBaseScalarDataItem;
};

#endif // POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_H
