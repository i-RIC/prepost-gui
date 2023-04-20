#ifndef POST3DWINDOWPARTICLESBASEVECTORDATAITEM_H
#define POST3DWINDOWPARTICLESBASEVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include "../post3dwindowdataitem.h"

class Post3dWindowParticlesBaseVectorGroupDataItem;
class SolverDefinitionGridRelatedCondition;

class Post3dWindowParticlesBaseVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post3dWindowParticlesBaseVectorDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

private:
	Post3dWindowParticlesBaseVectorGroupDataItem* groupDataItem() const;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORDATAITEM_H
