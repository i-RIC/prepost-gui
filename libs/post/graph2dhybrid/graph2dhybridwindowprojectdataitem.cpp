#include "graph2dhybridwindow.h"
#include "graph2dhybridwindowactionmanager.h"
#include "graph2dhybridwindowcontrolwidget.h"
#include "graph2dhybridwindowdatamodel.h"
#include "graph2dhybridwindowobjectbrowser.h"
#include "graph2dhybridwindowobjectbrowserview.h"
#include "graph2dhybridwindowprojectdataitem.h"
#include "graph2dhybridwindowview.h"
#include "graph2dhybridwindowview.h"

#include <guibase/marginwidget.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dHybridWindowProjectDataItem::Graph2dHybridWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow)
	: PostProcessorWindowProjectDataItem(parent, parentWindow)
{
	Graph2dHybridWindow* w = new Graph2dHybridWindow(parentWindow, index, this);
	m_window = w;
	w->m_dataModel = new Graph2dHybridWindowDataModel(w, this);
	w->m_dataModel->setObjectBrowserView(w->m_objectBrowser->view());

	QWidget* oldCenter = w->centralWidget();
	MarginWidget* marginw = new MarginWidget(parentWindow);
	marginw->setMargin(8);
	marginw->setWidget(w->m_dataModel->view());
	w->setCentralWidget(marginw);

	w->m_objectBrowser->setModel(w->m_dataModel->itemModel());
	// first, objectbrowser view is expanded.
	w->m_objectBrowser->expandAll();
	w->m_actionManager->connectWithDataModel();

	connect(w->m_objectBrowser->view(), SIGNAL(requestShowPropertyDialog(QModelIndex)), w->m_dataModel, SLOT(showPropertyDialog(QModelIndex)));
	connect(w->m_objectBrowser->view(), SIGNAL(pressed(QModelIndex,QPoint)), w->m_dataModel, SLOT(handleObjectBrowserPress(QModelIndex,QPoint)));

	connect(w->m_controlWidget, SIGNAL(dataSourceButtonClicked()), w->m_dataModel, SLOT(dataSourceSetting()));
	connect(w->m_controlWidget, SIGNAL(axisButtonClicked()), w->m_dataModel, SLOT(axisSetting()));
	connect(w->m_controlWidget, SIGNAL(drawButtonClicked()), w->m_dataModel, SLOT(drawSetting()));
	connect(w->m_controlWidget, SIGNAL(markerButtonClicked()), w->m_dataModel, SLOT(markerSettiing()));
	connect(w->m_controlWidget, SIGNAL(snapshotButtonClicked()), w->m_dataModel, SLOT(specialSnapshot()));
	connect(w->m_controlWidget, SIGNAL(copyButtonClicked()), w->m_dataModel, SLOT(copyCalculationResult()));
//	connect(w->m_controlWidget, SIGNAL(csvExportButtonClicked()), w->m_dataModel, SLOT(exportCsv()));
	connect(w->m_controlWidget, SIGNAL(csvExportButtonClicked()), w->m_dataModel, SLOT(specialCsvExport()));

	connect(w->controlWidget(), SIGNAL(indexValueChanged(int)), w->m_dataModel, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(iValueChanged(int)), w->m_dataModel, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(jValueChanged(int)), w->m_dataModel, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(kValueChanged(int)), w->m_dataModel, SLOT(sliderChanged()));
	delete oldCenter;
}

void Graph2dHybridWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(m_window);

	QDomNode modelNode = iRIC::getChildNode(node, "DataModel");
	if (! modelNode.isNull()) {
		w->m_dataModel->loadFromProjectMainFile(modelNode);
		w->m_dataModel->reflectExpandState(w->m_objectBrowser->view());
	}
	QDomNode widgetNode = iRIC::getChildNode(node, "Widget");
	if (! widgetNode.isNull()) {
		w->controlWidget()->loadFromProjectMainFile(widgetNode);
	}

	ProjectDataItem::loadWindowGeometry(w->parentWidget(), node);
}

void Graph2dHybridWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", "graph2dhybridwindow");
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(m_window);
	iRIC::setIntAttribute(writer, "index", w->index());

	ProjectDataItem::writeWindowGeometry(w->parentWidget(), writer);

	writer.writeStartElement("DataModel");
	w->m_dataModel->updateExpandState(w->m_objectBrowser->view());
	w->m_dataModel->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("Widget");
	w->controlWidget()->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

bool Graph2dHybridWindowProjectDataItem::setupInitialSetting()
{
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(m_window);
	return w->m_dataModel->setupInitialSetting();
}
