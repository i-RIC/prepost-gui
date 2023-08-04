#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_H

#include "../post3dwindowfacesettingcontainer.h"
#include "../post3dwindowdataitem.h"

#include <string>

class ColorMapSettingContainer;
class Post3dWindowZoneDataItem;
class PostZoneDataContainer;
class ValueRangeContainer;

class Post3dWindowContourGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

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

	void updateActorSetting() override;

	const ValueRangeContainer& valueRange() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	PostZoneDataContainer* data() const;

	std::vector<Post3dWindowFaceSettingContainer> faces() const;
	void setFaces(const std::vector<Post3dWindowFaceSettingContainer>& ranges);

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

	class Impl;
	Impl* impl;

	class Setting;
	class SettingEditWidget;

	class PropertyDialog;
	class SetFacesCommand;
	class UpdateActorSettingCommand;

public:
	friend class Post3dWindowContourDataItem;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_H
