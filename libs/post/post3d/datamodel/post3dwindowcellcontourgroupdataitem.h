#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_H

#include "../post3dwindowcellrangesettingcontainer.h"
#include "../post3dwindowdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>

#include <string>

class ColorMapSettingToolBarWidget;
class Post3dWindowZoneDataItem;
class PostZoneDataContainer;
class ValueRangeContainer;

class vtkActor2D;

class Post3dWindowCellContourGroupDataItem : public Post3dWindowDataItem
{
public:
	Post3dWindowCellContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p);
	~Post3dWindowCellContourGroupDataItem();

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

	std::vector<Post3dWindowCellRangeSettingContainer> ranges() const;
	void setRanges(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges);

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

	std::string m_target;

	ColorMapSettingContainer m_colorMapSetting;
	vtkActor2D* m_legendActor;

	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;

	class SetRangesCommand;
	class SettingEditWidget;

public:
	friend class Post3dWindowCellContourDataItem;
};

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_H
