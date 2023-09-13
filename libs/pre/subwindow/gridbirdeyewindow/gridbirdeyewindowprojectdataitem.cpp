#include "../../datamodel/preprocessorgriddataitem.h"
#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowdatamodel.h"
#include "gridbirdeyeobjectbrowser.h"
#include "gridbirdeyeobjectbrowserview.h"
#include "gridbirdeyewindowgraphicsview.h"
#include "gridbirdeyewindowprojectdataitem.h"

#include <misc/xmlsupport.h>

GridBirdEyeWindowProjectDataItem::GridBirdEyeWindowProjectDataItem(PreProcessorGridDataItem* parent, QWidget* parentWindow) :
	ProjectDataItem {parent},
	m_geometry {},
	m_window {new GridBirdEyeWindow(parentWindow, parent)}
{
	auto w = m_window;
	w->m_projectDataItem = this;
	w->m_dataModel = new GridBirdEyeWindowDataModel(w, this);
	w->m_dataModel->setObjectBrowserView(w->m_objectBrowser->view());

	QWidget* oldCenter = w->centralWidget();
	w->setCentralWidget(w->m_dataModel->graphicsView());
	w->m_objectBrowser->setModel(w->m_dataModel->itemModel());
	// first, objectbrowser view is expanded.
	w->m_objectBrowser->expandAll();
	// w->m_actionManager->connectWithDataModel();

	connect(w->m_objectBrowser->view(), SIGNAL(requestDeleteItem(QModelIndex)), w->m_dataModel, SLOT(deleteItem(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestShowAddDialog(QModelIndex)), w->m_dataModel, SLOT(showAddDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(requestShowPropertyDialog(QModelIndex)), w->m_dataModel, SLOT(showPropertyDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(pressed(QModelIndex,QPoint)), w->m_dataModel, SLOT(handleObjectBrowserPress(QModelIndex,QPoint)));
	delete oldCenter;
}

GridBirdEyeWindowProjectDataItem::~GridBirdEyeWindowProjectDataItem()
{
	if (m_window != nullptr) {
		auto w = m_window;
		w->m_projectDataItem = nullptr;
		m_window = nullptr;
		delete w->parentWidget();
	}}

GridBirdEyeWindow* GridBirdEyeWindowProjectDataItem::window() const
{
	return m_window;
}

void GridBirdEyeWindowProjectDataItem::informWindowClose()
{
	if (m_window != nullptr) {
		m_window = nullptr;
		dynamic_cast<PreProcessorGridDataItem*>(parent())->informBirdEyeWindowClose();
	}
}

void GridBirdEyeWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	/// object browser visibility
	m_window->objectBrowser()->setVisible(iRIC::getBooleanAttribute(node, "objectBrowserVisible", true));

	/// load background color
	QColor col = ProjectDataItem::loadBackgroundColor(node, QColor(Qt::white));
	m_window->setBackgroundColor(col);

	m_geometry.setWidget(m_window->parentWidget());
	m_geometry.load(node);

	QDomNode cameraNode = iRIC::getChildNode(node, "Camera");
	if (! cameraNode.isNull()) {
		vtkCamera* camera = m_window->m_dataModel->graphicsView()->mainRenderer()->GetActiveCamera();
		ProjectDataItem::loadCamera(camera, cameraNode);
	}
	/// load Object Browser settings
	QDomNode modelNode = iRIC::getChildNode(node, "DataModel");
	if (! modelNode.isNull()) {
		m_window->m_dataModel->loadFromProjectMainFile(modelNode);
		m_window->m_dataModel->reflectExpandState(m_window->m_objectBrowser->view());
	}
	m_window->m_dataModel->graphicsView()->render();
}

void GridBirdEyeWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	ProjectDataItem::writeBackgroundColor(m_window->backgroundColor(), writer);

	/// object browser visibility
	iRIC::setBooleanAttribute(writer, "objectBrowserVisible", m_window->objectBrowser()->isVisible());

	/// save background color
	ProjectDataItem::writeBackgroundColor(m_window->backgroundColor(), writer);

	/// save Window geometry
	m_geometry.setWidget(m_window->parentWidget());
	m_geometry.save(writer);

	/// save Camera settings
	writer.writeStartElement("Camera");
	vtkCamera* camera = m_window->m_dataModel->graphicsView()->mainRenderer()->GetActiveCamera();
	ProjectDataItem::saveCamera(camera, writer);
	writer.writeEndElement();

	/// save Object Browser setings
	writer.writeStartElement("DataModel");
	m_window->m_dataModel->updateExpandState(m_window->m_objectBrowser->view());
	m_window->m_dataModel->saveToProjectMainFile(writer);
	writer.writeEndElement();
}
