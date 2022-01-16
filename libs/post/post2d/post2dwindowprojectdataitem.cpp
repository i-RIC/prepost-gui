#include "datamodel/post2dwindowrootdataitem.h"
#include "post2dobjectbrowser.h"
#include "post2dobjectbrowserview.h"
#include "post2dwindow.h"
#include "post2dwindowactionmanager.h"
#include "post2dwindowdatamodel.h"
#include "post2dwindowgraphicsview.h"
#include "post2dwindowprojectdataitem.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkRenderer.h>

Post2dWindowProjectDataItem::Post2dWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow) :
	PostProcessorWindowProjectDataItem {parent, parentWindow},
	m_geometry {}
{
	Post2dWindow* w = new Post2dWindow(parentWindow, index, this);
	m_window = w;
	w->m_dataModel = new Post2dWindowDataModel(w, this);
	w->m_dataModel->setObjectBrowserView(w->m_objectBrowser->view());

	QWidget* oldCenter = w->centralWidget();
	w->setCentralWidget(w->m_dataModel->graphicsView());
	w->m_objectBrowser->setModel(w->m_dataModel->itemModel());

	Post2dWindowRootDataItem* rItem = dynamic_cast<Post2dWindowRootDataItem*>(w->m_dataModel->m_rootDataItem);
	w->propertyBrowser()->view()->setTargetDataItem(rItem->attributeBrowserTargetDataItem());
	// first, objectbrowser view is expanded.
	w->m_objectBrowser->expandAll();
	w->m_dataModel->initCollapseState();
	w->m_actionManager->connectWithDataModel();

	connect(w->m_objectBrowser->view(), SIGNAL(requestDeleteItem(QModelIndex)), w->m_dataModel, SLOT(deleteItem(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestShowAddDialog(QModelIndex)), w->m_dataModel, SLOT(showAddDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestShowPropertyDialog(QModelIndex)), w->m_dataModel, SLOT(showPropertyDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(pressed(QModelIndex,QPoint)), w->m_dataModel, SLOT(handleObjectBrowserPress(QModelIndex,QPoint)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestDeleteImage(QModelIndex)), projectData()->mainfile(), SLOT(deleteImage(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestMoveUpImage(QModelIndex)), projectData()->mainfile(), SLOT(moveUpImage(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestMoveDownImage(QModelIndex)), projectData()->mainfile(), SLOT(moveDownImage(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestDeleteMeasuredData(QModelIndex)), projectData()->mainfile(), SLOT(deleteMeasuredData(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestMoveUpMeasuredData(QModelIndex)), projectData()->mainfile(), SLOT(moveUpMeasuredData(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestMoveDownMeasuredData(QModelIndex)), projectData()->mainfile(), SLOT(moveDownMeasuredData(QModelIndex)));
	delete oldCenter;
}

QString Post2dWindowProjectDataItem::windowId() const
{
	return QString("post2dwindow%1").arg(m_window->index());
}

void Post2dWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(m_window);

	/// load background color
	QColor col = ProjectDataItem::loadBackgroundColor(node, QColor(Qt::white));
	w->setBackgroundColor(col);
	/// load Window settings
	m_geometry.setWidget(w->parentWidget());
	m_geometry.load(node);
	/// load Object Browser settings
	QDomNode modelNode = iRIC::getChildNode(node, "DataModel");
	if (! modelNode.isNull()) {
		w->m_dataModel->loadFromProjectMainFile(modelNode);
		w->m_dataModel->reflectExpandState(w->m_objectBrowser->view());
	}
	/// load Camera settings
	QDomNode cameraNode = iRIC::getChildNode(node, "Camera");
	if (! cameraNode.isNull()) {
		vtkCamera* camera = w->m_dataModel->graphicsView()->mainRenderer()->GetActiveCamera();
		ProjectDataItem::loadCamera(camera, cameraNode);
	}
	w->m_dataModel->graphicsView()->ResetCameraClippingRange();
}

void Post2dWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", "post2dwindow");
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(m_window);
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
