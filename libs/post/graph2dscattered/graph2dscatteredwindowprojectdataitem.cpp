#include "graph2dscatteredwindow.h"
#include "graph2dscatteredwindowactionmanager.h"
#include "graph2dscatteredwindowcontrolwidget.h"
#include "graph2dscatteredwindowdatamodel.h"
#include "graph2dscatteredwindowobjectbrowser.h"
#include "graph2dscatteredwindowobjectbrowserview.h"
#include "graph2dscatteredwindowprojectdataitem.h"
#include "graph2dscatteredwindowview.h"
#include "graph2dscatteredwindowview.h"

#include <guibase/marginwidget.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dScatteredWindowProjectDataItem::Graph2dScatteredWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow) :
	PostProcessorWindowProjectDataItem(parent, parentWindow),
	m_geometry {}
{
	Graph2dScatteredWindow* w = new Graph2dScatteredWindow(parentWindow, index, this);
	m_window = w;
	w->m_dataModel = new Graph2dScatteredWindowDataModel(w, this);
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

void Graph2dScatteredWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Graph2dScatteredWindow* w = dynamic_cast<Graph2dScatteredWindow*>(m_window);

	QDomNode modelNode = iRIC::getChildNode(node, "DataModel");
	if (! modelNode.isNull()) {
		w->m_dataModel->loadFromProjectMainFile(modelNode);
		w->m_dataModel->reflectExpandState(w->m_objectBrowser->view());
	}
	m_geometry.setWidget(w->parentWidget());
	m_geometry.load(node);
}

void Graph2dScatteredWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("type", "graph2dscatteredwindow");
	Graph2dScatteredWindow* w = dynamic_cast<Graph2dScatteredWindow*>(m_window);
	iRIC::setIntAttribute(writer, "index", w->index());

	m_geometry.setWidget(w->parentWidget());
	m_geometry.save(writer);

	writer.writeStartElement("DataModel");
	w->m_dataModel->updateExpandState(w->m_objectBrowser->view());
	w->m_dataModel->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

bool Graph2dScatteredWindowProjectDataItem::setupInitialSetting()
{
	Graph2dScatteredWindow* w = dynamic_cast<Graph2dScatteredWindow*>(m_window);
	return w->m_dataModel->setupInitialSetting();
}
