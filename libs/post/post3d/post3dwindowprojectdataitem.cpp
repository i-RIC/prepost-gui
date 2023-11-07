#include "post3dobjectbrowser.h"
#include "post3dobjectbrowserview.h"
#include "post3dwindow.h"
#include "post3dwindowactionmanager.h"
#include "post3dwindowdatamodel.h"
#include "post3dwindowgraphicsview.h"
#include "post3dwindowprojectdataitem.h"

#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkRenderer.h>

Post3dWindowProjectDataItem::Post3dWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow) :
	PostProcessorWindowProjectDataItem {parent, parentWindow},
	m_geometry {}
{
	Post3dWindow* w = new Post3dWindow(parentWindow, index, this);
	m_window = w;
	w->m_dataModel = new Post3dWindowDataModel(w, this);
	w->m_dataModel->setObjectBrowserView(w->m_objectBrowser->view());

	QWidget* oldCenter = w->centralWidget();
	w->setCentralWidget(w->m_dataModel->graphicsView());
	w->m_objectBrowser->setModel(w->m_dataModel->itemModel());
	// first, objectbrowser view is expanded.
	w->m_objectBrowser->expandAll();
	w->m_actionManager->connectWithDataModel();

	connect(w->m_objectBrowser->view(), SIGNAL(requestDeleteItem(QModelIndex)), w->m_dataModel, SLOT(deleteItem(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestShowAddDialog(QModelIndex)), w->m_dataModel, SLOT(showAddDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestShowPropertyDialog(QModelIndex)), w->m_dataModel, SLOT(showPropertyDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(pressed(QModelIndex,QPoint)), w->m_dataModel, SLOT(handleObjectBrowserPress(QModelIndex,QPoint)));
	delete oldCenter;
}

QString Post3dWindowProjectDataItem::windowId() const
{
	return QString("post3dwindow%1").arg(m_window->index());
}

void Post3dWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto w = dynamic_cast<Post3dWindow*>(m_window);

	/// load background color
	QColor col = ProjectDataItem::loadBackgroundColor(node, QColor(Qt::white));
	w->setBackgroundColor(col);
	/// load Window settings
	m_geometry.setWidget(w->parentWidget());
	m_geometry.load(node);

	/// load Camera settings
	QDomNode cameraNode = iRIC::getChildNode(node, "Camera");
	if (! cameraNode.isNull()) {
		vtkCamera* camera = w->m_dataModel->graphicsView()->mainRenderer()->GetActiveCamera();
		ProjectDataItem::loadCamera(camera, cameraNode);
	}

	/// load Object Browser settings
	QDomNode modelNode = iRIC::getChildNode(node, "DataModel");
	if (! modelNode.isNull()) {
		w->m_dataModel->loadFromProjectMainFile(modelNode);
		w->m_dataModel->reflectExpandState(w->m_objectBrowser->view());
	}
	w->m_dataModel->graphicsView()->render();
}

void Post3dWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", "post3dwindow");
	Post3dWindow* w = dynamic_cast<Post3dWindow*>(m_window);
	iRIC::setIntAttribute(writer, "index", w->index());

	/// save background color
	ProjectDataItem::writeBackgroundColor(w->backgroundColor(), writer);

	/// save Window geometry
	m_geometry.setWidget(w->parentWidget());
	m_geometry.save(writer);

	/// save Camera settings
	writer.writeStartElement("Camera");
	vtkCamera* camera = w->m_dataModel->graphicsView()->mainRenderer()->GetActiveCamera();
	ProjectDataItem::saveCamera(camera, writer);
	writer.writeEndElement();

	/// save Object Browser setings
	writer.writeStartElement("DataModel");
	w->m_dataModel->updateExpandState(w->m_objectBrowser->view());
	w->m_dataModel->saveToProjectMainFile(writer);
	writer.writeEndElement();
}
