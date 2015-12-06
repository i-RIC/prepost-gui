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
	Post2dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent);
	void handleStandardItemChange() override;
	const std::string& name() const {return m_name;}
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

signals:
	void changed(Post2dWindowNodeVectorParticleDataItem*);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	std::string m_name;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEDATAITEM_H
