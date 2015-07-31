#include "post2dbirdeyeobjectbrowser.h"
#include "post2dbirdeyeobjectbrowserview.h"
#include "post2dbirdeyewindow.h"
#include "post2dbirdeyewindowactionmanager.h"
#include "post2dbirdeyewindowdatamodel.h"
#include "post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowprojectdataitem.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkRenderer.h>

Post2dBirdEyeWindowProjectDataItem::Post2dBirdEyeWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow) :
	PostProcessorWindowProjectDataItem {parent, parentWindow}
{
	Post2dBirdEyeWindow* w = new Post2dBirdEyeWindow(parentWindow, index, this);
	m_window = w;
	w->m_dataModel = new Post2dBirdEyeWindowDataModel(w, this);
	w->m_dataModel->setObjectBrowserView(w->m_objectBrowser->view());

	QWidget* oldCenter = w->centralWidget();
	w->setCentralWidget(w->m_dataModel->graphicsView());
	w->m_objectBrowser->setModel(w->m_dataModel->itemModel());
	// first, objectbrowser view is expanded.
	w->m_objectBrowser->expandAll();
	w->m_actionManager->connectWithDataModel();

	connect(w->m_objectBrowser->view(), SIGNAL(requestShowPropertyDialog(QModelIndex)), w->m_dataModel, SLOT(showPropertyDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(pressed(QModelIndex,QPoint)), w->m_dataModel, SLOT(handleObjectBrowserPress(QModelIndex,QPoint)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestDeleteImage(QModelIndex)), projectData()->mainfile(), SLOT(deleteImage(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestMoveUpImage(QModelIndex)), projectData()->mainfile(), SLOT(moveUpImage(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestMoveDownImage(QModelIndex)), projectData()->mainfile(), SLOT(moveDownImage(QModelIndex)));
	delete oldCenter;
}

void Post2dBirdEyeWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dBirdEyeWindow* w = dynamic_cast<Post2dBirdEyeWindow*>(m_window);

	/// load background color
	QColor col = ProjectDataItem::loadBackgroundColor(node, QColor(Qt::white));
	w->setBackgroundColor(col);
	/// load Window settings
	ProjectDataItem::loadWindowGeometry(w->parentWidget(), node);
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
}

void Post2dBirdEyeWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", "post2dbirdeyewindow");
	Post2dBirdEyeWindow* w = dynamic_cast<Post2dBirdEyeWindow*>(m_window);
	iRIC::setIntAttribute(writer, "index", w->index());

	/// save background color
	ProjectDataItem::writeBackgroundColor(w->backgroundColor(), writer);

	/// save Window geometry
	ProjectDataItem::writeWindowGeometry(w->parentWidget(), writer);

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
