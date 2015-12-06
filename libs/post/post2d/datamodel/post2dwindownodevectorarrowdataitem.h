#ifndef POST2DWINDOWNODEVECTORARROWDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class SolverDefinitionGridAttribute;
class QAction;

class Post2dWindowNodeVectorArrowDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dWindowNodeVectorArrowDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
};

#endif // POST2DWINDOWNODEVECTORARROWDATAITEM_H
