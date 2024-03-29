#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_H

#include "../post3dwindowfacesettingcontainer.h"
#include "../post3dwindowdataitem.h"

#include <memory>
#include <string>

class ColorMapSettingContainer;
class Post3dWindowZoneDataItem;
class v4PostZoneDataContainer;
class ValueRangeContainer;

class Post3dWindowContourGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p);
	~Post3dWindowContourGroupDataItem();

	void update();
	void updateColorMapVisibility();

	const std::string& target() const;
	void gatherActiveColorMapLegends(std::vector<ColorMapLegendSettingContainerI*>* legends) override;

public slots:
	void showPropertyDialog() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QDialog* propertyDialog(QWidget* p) override;

	void updateActorSetting() override;
	void updateVisibility(bool visible) override;

	const ValueRangeContainer& valueRange() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	v4PostZoneDataContainer* data() const;

	std::vector<Post3dWindowFaceSettingContainer> faces() const;
	void setFaces(const std::vector<Post3dWindowFaceSettingContainer>& ranges);

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

	class Impl;
	std::unique_ptr<Impl> impl;

	class Setting;
	class SettingEditWidget;

	class SetFacesCommand;

public:
	friend class Post3dWindowContourDataItem;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_H
