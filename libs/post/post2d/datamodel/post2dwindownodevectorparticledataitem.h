#ifndef POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H

#include "../post2dwindowdataitem.h"

class QAction;
class QColor;

class Post2dWindowNodeVectorParticleDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowNodeVectorParticleDataItem(const QString& name, const QString& caption, Post2dWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);

signals:
	void changed(Post2dWindowNodeVectorParticleDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode&) {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) {}

private:
	QString m_name;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
