#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_H

#include "../post3dwindowfacesettingcontainer.h"
#include "../post3dwindowdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>

#include <string>

class Post3dWindowZoneDataItem;
class PostZoneDataContainer;
class ValueRangeContainer;

class Post3dWindowContourGroupDataItem : public Post3dWindowDataItem
{
public:
	Post3dWindowContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p);
	~Post3dWindowContourGroupDataItem();

	void update();

	const std::string& target() const;

public slots:
	void showPropertyDialog() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QDialog* propertyDialog(QWidget* p) override;

	void updateActorSettings();

	const ValueRangeContainer& valueRange() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	PostZoneDataContainer* data() const;

	std::vector<Post3dWindowFaceSettingContainer> faces() const;
	void setFaces(const std::vector<Post3dWindowFaceSettingContainer>& ranges);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doHandleResize(VTKGraphicsView* v) override;

	std::string m_target;

	ColorMapSettingContainer m_colorMapSetting;
	vtkActor2D* m_legendActor;

	class PropertyDialog;
	class SetFacesCommand;
	class UpdateActorSettingsCommand;

public:
	friend class Post3dWindowContourDataItem;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_H
