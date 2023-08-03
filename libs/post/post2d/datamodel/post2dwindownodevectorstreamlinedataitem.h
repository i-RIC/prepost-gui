#ifndef POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowNodeVectorStreamlineGroupDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowNodeVectorStreamlineDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void showPropertyDialog() override;

private:
	QDialog* propertyDialog(QWidget* parent) override;

	Post2dWindowZoneDataItem* zoneDataItem() const;
	Post2dWindowNodeVectorStreamlineGroupDataItem* groupDataItem() const;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
