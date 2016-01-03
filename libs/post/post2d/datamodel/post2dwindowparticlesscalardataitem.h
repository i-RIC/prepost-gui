#ifndef POST2DWINDOWPARTICLESSCALARDATAITEM_H
#define POST2DWINDOWPARTICLESSCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowParticlesScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	virtual ~Post2dWindowParticlesScalarDataItem();

	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);
};

#endif // POST2DWINDOWPARTICLESSCALARDATAITEM_H
