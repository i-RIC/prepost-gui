#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowactionmanager.h"
#include "graph2dverificationwindowcontrolwidget.h"
//{{
#include "graph2dverificationwindowtopwidget.h"
//}}
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

	//{{
	connect(w->topWidget(), SIGNAL(graphTypeChanged(int)), w->m_dataModel, SLOT(setType(int)));
	connect(w->controlWidget(), SIGNAL(settingButtonClicked()), w->m_dataModel, SLOT(dataSourceSetting()));
	connect(w->controlWidget(), SIGNAL(exportButtonClicked()), w->m_dataModel, SLOT(exportData()));
	//}}
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
#if 0
	QDomNode widgetNode = iRIC::getChildNode(node, "Widget");
	if (! widgetNode.isNull()) {
		w->controlWidget()->loadFromProjectMainFile(widgetNode);
	}
#endif
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

#if 0
	writer.writeStartElement("Widget");
	w->controlWidget()->saveToProjectMainFile(writer);
	writer.writeEndElement();
#endif
}

#include <gui/verification/verificationsettingdialog.h>

bool Graph2dVerificationWindowProjectDataItem::setupInitialSetting()
{
	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(m_window);
	return w->m_dataModel->setupInitialSetting();


	//{{
	//VerificationSettingDialog dialog(m_window);
	//dialog.setPostSolutionInfo(m_postSolutionInfo);
	//dialog.setMeasuredDatas(m_measuredData);

	//dialog.setTimeStep(m_timeStep);
	//dialog.setPostZoneDataContainer(m_activePostData);
	//dialog.setActiveResult(m_activeResult);

	//dialog.setMeasuredData(m_activeMeasuredData);
	//dialog.setActiveValue(m_activeValue);

	//int result = dialog.exec();
	//if (result == QDialog::Rejected) { return false; }

	//m_timeStep = dialog.timeStep();
	//m_activePostData = dialog.postZoneDataContainer();
	//m_activeResult = dialog.activeResult();
	//m_activeMeasuredData = dialog.measuredData();
	//m_activeValue = dialog.activeValue();
	//m_mainWindow->setCurrentStep(m_timeStep);

	//vtkPointSet* ps = m_activePostData->data();
	//vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);

	//ui->typeComboBox->blockSignals(true);
	//ui->typeComboBox->clear();

	//if (sgrid == nullptr) {
	//	// unstructured grid.
	//}
	//else {
	//	// structured grid.
	//	ui->typeComboBox->addItem(tr("Stream-wise Distance vs. Measured Values and Calculation Result"));
	//	ui->typeComboBox->addItem(tr("Stream-wise Distance vs. Residual Errors"));
	//}
	//ui->typeComboBox->addItem(tr("Measured Values vs. Calculation Result"));
	//ui->typeComboBox->addItem(tr("Measured Values vs. Residual Errors"));

	//ui->typeComboBox->blockSignals(false);
	//if (sgrid == nullptr) {
	//	switch (m_graphType) {
	//	case gtMVvsCR:
	//		ui->typeComboBox->setCurrentIndex(0);
	//		break;
	//	case gtMVvsError:
	//		ui->typeComboBox->setCurrentIndex(1);
	//		break;
	//	default:
	//		m_graphType = gtMVvsCR;
	//		ui->typeComboBox->setCurrentIndex(0);
	//		break;
	//	}
	//}
	//else {
	//	switch (m_graphType) {
	//	case gtSWDvsValues:
	//		ui->typeComboBox->setCurrentIndex(0);
	//		break;
	//	case gtSWDvsError:
	//		ui->typeComboBox->setCurrentIndex(1);
	//		break;
	//	case gtMVvsCR:
	//		ui->typeComboBox->setCurrentIndex(2);
	//		break;
	//	case gtMVvsError:
	//		ui->typeComboBox->setCurrentIndex(3);
	//		break;
	//	}
	//}
	//updateGraph();
	//return true;
	//}}
}
