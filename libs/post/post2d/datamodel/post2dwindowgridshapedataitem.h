#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/point/pointcontroller.h>
#include <guibase/gridshape/gridshapesettingcontainer.h>

class Post2dWindowGridDataItemI;
class PostCrosssectionWindow;

class Post2dWindowGridShapeDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowGridShapeDataItem();

	void update();
	void handleStandardItemDoubleClicked() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

private slots:
	void openCrossSectionWindow();
	void openVerticalCrossSectionWindow();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void setupActors();
	void doUpdateActorSetting() override;
	PostCrosssectionWindow* createCrosssectionWindow() const;

	Post2dWindowGridDataItemI* gridDataItem() const;

	GridShapeSettingContainer m_setting;

	vtkIdType m_selectedpointId;
	PointController m_pointController;
	QPoint m_dragStartPoint;

private:
	QMenu* m_rightClickingMenu;
	QAction* m_openXsectionWindowAction;
	QAction* m_openVXsectionWindowAction;

	class SettingEditWidget;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_H
