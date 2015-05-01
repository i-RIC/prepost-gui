#ifndef POST2DWINDOWNODESCALARDATAITEM_H
#define POST2DWINDOWNODESCALARDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridRelatedCondition;
class PostZoneDataContainer;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class Post2dWindowNodeScalarDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowNodeScalarDataItem(const QString& name, const QString& caption, Post2dWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);

signals:
	void changed(Post2dWindowNodeScalarDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode&) {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) {}

private:
	QString m_name;
};

#endif // POST2DWINDOWNODESCALARDATAITEM_H
