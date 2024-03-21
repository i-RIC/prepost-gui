#include "graph2dhybridwindow.h"
#include "graph2dhybridwindowactionmanager.h"
#include "graph2dhybridwindowcontrolwidget.h"
#include "graph2dhybridwindowdatamodel.h"
#include "graph2dhybridwindowobjectbrowser.h"
#include "graph2dhybridwindowobjectbrowserview.h"
#include "graph2dhybridwindowprojectdataitem.h"
#include "graph2dhybridwindowview.h"
#include "graph2dhybridwindowview.h"

#include <guibase/widget/marginwidget.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dHybridWindowProjectDataItem::Graph2dHybridWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow) :
	PostProcessorWindowProjectDataItem(parent, parentWindow),
	m_geometry {}
{
	Graph2dHybridWindow* w = new Graph2dHybridWindow(parentWindow, index, this);
	m_window = w;
	w->m_dataModel = new Graph2dHybridWindowDataModel(w, this);
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

	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::dataSourceButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::dataSourceSetting);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::axisButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::axisSetting);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::drawButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::drawSetting);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::markerButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::markerSettiing);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::fontButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::fontSetting);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::snapshotButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::specialSnapshot);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::copyButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::copyCalculationResult);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::csvExportButtonClicked, w->m_dataModel, &Graph2dHybridWindowDataModel::specialCsvExport);

	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::indexValueChanged, w->m_dataModel, &Graph2dHybridWindowDataModel::sliderChanged);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::iValueChanged, w->m_dataModel, &Graph2dHybridWindowDataModel::sliderChanged);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::jValueChanged, w->m_dataModel, &Graph2dHybridWindowDataModel::sliderChanged);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::kValueChanged, w->m_dataModel, &Graph2dHybridWindowDataModel::sliderChanged);
	connect(w->m_controlWidget, &Graph2dHybridWindowControlWidget::polyLineChanged, w->m_dataModel, &Graph2dHybridWindowDataModel::polyLineChanged);
	delete oldCenter;
}

QString Graph2dHybridWindowProjectDataItem::windowId() const
{
	return QString("graph2dhybridwindow%1").arg(m_window->index());
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
	m_geometry.setWidget(w->parentWidget());
	m_geometry.load(node);
}

void Graph2dHybridWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", "graph2dhybridwindow");
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(m_window);
	iRIC::setIntAttribute(writer, "index", w->index());

	m_geometry.setWidget(w->parentWidget());
	m_geometry.save(writer);

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
