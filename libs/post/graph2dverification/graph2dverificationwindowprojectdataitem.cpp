#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowactionmanager.h"
#include "graph2dverificationwindowcontrolwidget.h"
#include "graph2dverificationwindowtopwidget.h"
#include "graph2dverificationwindowdatamodel.h"
#include "graph2dverificationwindowobjectbrowser.h"
#include "graph2dverificationwindowobjectbrowserview.h"
#include "graph2dverificationwindowprojectdataitem.h"
#include "graph2dverificationwindowview.h"
#include "graph2dverificationwindowview.h"

#include <guibase/widget/marginwidget.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dVerificationWindowProjectDataItem::Graph2dVerificationWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow) :
	PostProcessorWindowProjectDataItem(parent, parentWindow),
	m_geometry {}
{
	Graph2dVerificationWindow* w = new Graph2dVerificationWindow(parentWindow, index, this);
	m_window = w;
	w->m_dataModel = new Graph2dVerificationWindowDataModel(w, this);
	w->m_dataModel->setObjectBrowserView(w->m_objectBrowser->view());

	QWidget* oldCenter = w->centralWidget();
	MarginWidget* marginw = new MarginWidget(parentWindow);
	marginw->setMarginPixels(8);
	marginw->setWidget(w->m_dataModel->view());
	w->setCentralWidget(marginw);

	w->m_objectBrowser->setModel(w->m_dataModel->itemModel());
	// first, objectbrowser view is expanded.
	w->m_objectBrowser->expandAll();
	w->m_actionManager->connectWithDataModel();

	connect(w->m_objectBrowser->view(), SIGNAL(requestShowPropertyDialog(QModelIndex)), w->m_dataModel, SLOT(showPropertyDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(pressed(QModelIndex,QPoint)), w->m_dataModel, SLOT(handleObjectBrowserPress(QModelIndex,QPoint)));

	connect(w->topWidget(), SIGNAL(graphTypeChanged(int)), w->m_dataModel, SLOT(setType(int)));
	connect(w->controlWidget(), SIGNAL(settingButtonClicked()), w->m_dataModel, SLOT(dataSourceSetting()));
	connect(w->controlWidget(), SIGNAL(exportButtonClicked()), w->m_dataModel, SLOT(exportData()));

	delete oldCenter;
}

void Graph2dVerificationWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(m_window);

	QDomNode modelNode = iRIC::getChildNode(node, "DataModel");
	if (! modelNode.isNull()) {
		w->m_dataModel->loadFromProjectMainFile(modelNode);
		w->m_dataModel->reflectExpandState(w->m_objectBrowser->view());
	}
	m_geometry.setWidget(w->parentWidget());
	m_geometry.load(node);
}

void Graph2dVerificationWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", "graph2dverificationwindow");
	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(m_window);
	iRIC::setIntAttribute(writer, "index", w->index());

	m_geometry.setWidget(w->parentWidget());
	m_geometry.save(writer);

	writer.writeStartElement("DataModel");
	w->m_dataModel->updateExpandState(w->m_objectBrowser->view());
	w->m_dataModel->saveToProjectMainFile(writer);
	writer.writeEndElement();

}

bool Graph2dVerificationWindowProjectDataItem::setupInitialSetting()
{
	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(m_window);
	return w->m_dataModel->setupInitialSetting();
}
