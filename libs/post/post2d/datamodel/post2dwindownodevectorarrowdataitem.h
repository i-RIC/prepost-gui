#ifndef POST2DWINDOWNODEVECTORARROWDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWDATAITEM_H

#include "../post2dwindowdataitem.h"

class SolverDefinitionGridRelatedCondition;
class QAction;

class Post2dWindowNodeVectorArrowDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowNodeVectorArrowDataItem(const QString& name, const QString& caption, Post2dWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);

signals:
	void changed(Post2dWindowNodeVectorArrowDataItem*);
protected:
	void doLoadFromProjectMainFile(const QDomNode& ){}
	void doSaveToProjectMainFile(QXmlStreamWriter& ){}
private:
	QString m_name;
};

#endif // POST2DWINDOWNODEVECTORARROWDATAITEM_H
