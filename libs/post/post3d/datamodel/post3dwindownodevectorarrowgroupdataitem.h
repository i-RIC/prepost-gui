#ifndef POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include "post3dwindownodevectorarrowdataitem.h"

#include <guicore/arrows/arrowssettingcontainer.h>
#include <guibase/vtktool/vtkarrowlegendactors.h>

#include <set>
#include <unordered_map>

class BoolContainerWidget;
class ColorMapSettingContainer;
class Post3dWindowNodeVectorArrowTopDataItem;
class PostZoneDataContainer;

class Post3dWindowNodeVectorArrowGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorArrowGroupDataItem(const std::string& target, Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorArrowGroupDataItem();

	void update();

	const std::string& target() const;
	ColorMapSettingContainer* colorMapSetting(const std::string& name) const;
	const ArrowsSettingContainer& setting() const;
	PostZoneDataContainer* data() const;

	std::vector<Post3dWindowNodeVectorArrowDataItem::Setting> faceSettings();
	void setFaceSettings(const std::vector<Post3dWindowNodeVectorArrowDataItem::Setting>& settings);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

public slots:
	void showPropertyDialog() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	QDialog* propertyDialog(QWidget* p) override;

	void updateActorSetting() override;
	Post3dWindowNodeVectorArrowTopDataItem* topDataItem() const;
	std::set<ColorMapSettingContainer*> activeColorMaps() const;

	std::string m_target;
	ArrowsSettingContainer m_setting;

	vtkActor2D* m_legendActor;
	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;
	std::vector<vtkActor2D*> m_colorMapActors;

	BoolContainerWidget* m_lengthLegendVisibilityWidget;

	class PropertyDialog;
	class SetFaceSettingsCommand;
	class UpdateActorSettingCommand;
	class SettingEditWidget;

	friend class Post3dWindowNodeVectorArrowDataItem;
};

#endif // POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_H
