#ifndef POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class QAction;
class QColor;

class Post2dWindowNodeVectorParticleDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dWindowNodeVectorParticleDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
