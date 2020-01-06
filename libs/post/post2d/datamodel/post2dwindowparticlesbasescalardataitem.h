#ifndef POST2DWINDOWPARTICLESBASESCALARDATAITEM_H
#define POST2DWINDOWPARTICLESBASESCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowParticlesBaseScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dWindowParticlesBaseScalarDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
};

#endif // POST2DWINDOWPARTICLESBASESCALARDATAITEM_H
