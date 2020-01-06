#ifndef POST2DWINDOWPARTICLESBASEVECTORDATAITEM_H
#define POST2DWINDOWPARTICLESBASEVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class SolverDefinitionGridRelatedCondition;

class Post2dWindowParticlesBaseVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORDATAITEM_H
