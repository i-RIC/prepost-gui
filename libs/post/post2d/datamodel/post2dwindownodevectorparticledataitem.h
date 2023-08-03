#ifndef POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowNodeVectorParticleGroupDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowNodeVectorParticleDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

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
	Post2dWindowNodeVectorParticleGroupDataItem* groupDataItem() const;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
