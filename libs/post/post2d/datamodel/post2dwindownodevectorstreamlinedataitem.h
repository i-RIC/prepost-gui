#ifndef POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H

#include "../post2dwindowdataitem.h"

class QAction;
class QColor;

class Post2dWindowNodeVectorStreamlineDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowNodeVectorStreamlineDataItem(const QString& name, const QString& caption, Post2dWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);

signals:
	void changed(Post2dWindowNodeVectorStreamlineDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode& ){}
	void doSaveToProjectMainFile(QXmlStreamWriter& ){}

private:
	QString m_name;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
