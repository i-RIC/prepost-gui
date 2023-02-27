#ifndef POST2DWINDOWPARTICLESBASEVECTORDATAITEM_H
#define POST2DWINDOWPARTICLESBASEVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowParticlesBaseVectorGroupDataItem;
class SolverDefinitionGridRelatedCondition;

class Post2dWindowParticlesBaseVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dWindowParticlesBaseVectorDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

private:
	Post2dWindowParticlesBaseVectorGroupDataItem* groupDataItem() const;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORDATAITEM_H
