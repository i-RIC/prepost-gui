#ifndef POST2DWINDOWPARTICLESVECTORDATAITEM_H
#define POST2DWINDOWPARTICLESVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class SolverDefinitionGridRelatedCondition;
class QAction;

class Post2dWindowParticlesVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowParticlesVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST2DWINDOWPARTICLESVECTORDATAITEM_H
