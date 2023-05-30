#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_H

#include "post2dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/polydata/polydatasetting.h>

#include <unordered_map>

class ColorMapSettingContainer;
class NamedGraphicWindowDataItem;
class Post2dWindowGridTypeDataItem;

class vtkActor;
class vtkActor2D;

class Post2dWindowPolyDataGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowPolyDataGroupDataItem(const std::string& name, Post2dWindowDataItem* parent);
	~Post2dWindowPolyDataGroupDataItem() override;

	std::string name() const;

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void setupActors();
	void update();
	void updateActorSetting() override;

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

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	ColorMapSettingContainer* activeSetting() const;
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;
	vtkPolyData* polyData() const;
	void updateCheckState();

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	PolyDataSetting m_setting;
	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;

	class SettingEditWidget;

public:
	friend class Post2dWindowPolyDataValueDataItem;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_H
