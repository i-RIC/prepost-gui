#ifndef POST2DWINDOWPOLYDATAVALUEDATAITEM_H
#define POST2DWINDOWPOLYDATAVALUEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowZoneDataItem;
class Post2dWindowPolyDataGroupDataItem;

class Post2dWindowPolyDataValueDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowPolyDataValueDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dWindowPolyDataValueDataItem() override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

private:
	Post2dWindowZoneDataItem* zoneDataItem() const;
	Post2dWindowPolyDataGroupDataItem* polyDataGroupDataItem() const;
};

#endif // POST2DWINDOWPOLYDATAVALUEDATAITEM_H
