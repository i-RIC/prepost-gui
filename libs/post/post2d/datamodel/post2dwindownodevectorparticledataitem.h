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
	void handleStandardItemChange() override;
	const QString& name() const {return m_name;}
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

signals:
	void changed(Post2dWindowNodeVectorParticleDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) override {}

private:
	QString m_name;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
