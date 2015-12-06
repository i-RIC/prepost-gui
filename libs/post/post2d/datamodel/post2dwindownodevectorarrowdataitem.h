#ifndef POST2DWINDOWNODEVECTORARROWDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWDATAITEM_H

#include "../post2dwindowdataitem.h"

class SolverDefinitionGridAttribute;
class QAction;

class Post2dWindowNodeVectorArrowDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post2dWindowNodeVectorArrowDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent);
	void handleStandardItemChange() override;
	const std::string& name() const {return m_name;}
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

signals:
	void changed(Post2dWindowNodeVectorArrowDataItem*);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	std::string m_name;
};

#endif // POST2DWINDOWNODEVECTORARROWDATAITEM_H
