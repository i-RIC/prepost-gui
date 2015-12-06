#ifndef POST2DWINDOWNODESCALARDATAITEM_H
#define POST2DWINDOWNODESCALARDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridAttribute;
class PostZoneDataContainer;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class Post2dWindowNodeScalarDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post2dWindowNodeScalarDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent);
	void handleStandardItemChange() override;
	const std::string& name() const {return m_name;}
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

signals:
	void changed(Post2dWindowNodeScalarDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) override {}

private:
	std::string m_name;
};

#endif // POST2DWINDOWNODESCALARDATAITEM_H
