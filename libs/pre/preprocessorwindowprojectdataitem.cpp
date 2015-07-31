#include "datamodel/preprocessorinputconditiondataitem.h"
#include "datamodel/preprocessorrootdataitem.h"
#include "preobjectbrowser.h"
#include "preobjectbrowserview.h"
#include "preprocessordatamodel.h"
#include "preprocessorgraphicsview.h"
#include "preprocessorwindow.h"
#include "preprocessorwindowactionmanager.h"
#include "preprocessorwindowprojectdataitem.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/project/projectmainfile.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>

#include <vtkCamera.h>
#include <vtkRenderer.h>

PreProcessorWindowProjectDataItem::PreProcessorWindowProjectDataItem(PreProcessorWindow* w, ProjectMainFile* parent) :
	ProjectDataItem(parent)
{
	m_preProcessorWindow = w;
	m_preProcessorWindow->setBackgroundColor(QColor(Qt::white));
}

void PreProcessorWindowProjectDataItem::initForSolverDefinition()
{
	PreProcessorWindow* parent = m_preProcessorWindow;
	parent->m_dataModel = new PreProcessorDataModel(parent, this);
	parent->m_dataModel->setObjectBrowserView(parent->m_objectBrowser->view());
	parent->m_objectBrowser->setModel(parent->m_dataModel->itemModel());
	// first, objectbrowser view is expanded.
	parent->m_objectBrowser->expandAll();
	parent->m_dataModel->updateOperationToolBar(QModelIndex(), parent);
	parent->m_isLastHiding = false;

	PreProcessorDataModel* model = dynamic_cast<PreProcessorDataModel*>(parent->m_dataModel);
	PreProcessorRootDataItem* rItem = dynamic_cast<PreProcessorRootDataItem*>(model->rootDataItem());
	parent->propertyBrowser()->view()->setTargetDataItem(rItem->attributeBrowserTargetDataItem());

	parent->m_dataModel->graphicsView()->resetCamera();
	createConnections();
}

void PreProcessorWindowProjectDataItem::createConnections()
{
	PreProcessorWindow* parent = m_preProcessorWindow;

	parent->m_actionManager->connectWithDataModel();
	// make connections between QTreeView and datamodel.
	connect(parent->m_objectBrowser->view(), SIGNAL(clicked(QModelIndex)), parent->m_dataModel, SLOT(handleObjectBrowserClick(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(doubleClicked(QModelIndex)), parent->m_dataModel, SLOT(handleObjectBrowserDoubleClick(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(pressed(QModelIndex, QPoint)), parent->m_dataModel, SLOT(handleObjectBrowserPress(QModelIndex, QPoint)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestDeleteItem(QModelIndex)), parent->m_dataModel, SLOT(deleteItem(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestMoveUpItem(QModelIndex)), parent->m_dataModel, SLOT(moveUpItem(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestMoveDownItem(QModelIndex)), parent->m_dataModel, SLOT(moveDownItem(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestShowPropertyDialog(QModelIndex)), parent->m_dataModel, SLOT(showPropertyDialog(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestDeleteImage(QModelIndex)), dynamic_cast<ProjectMainFile*>(this->parent()), SLOT(deleteImage(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestMoveUpImage(QModelIndex)), dynamic_cast<ProjectMainFile*>(this->parent()), SLOT(moveUpImage(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestMoveDownImage(QModelIndex)), dynamic_cast<ProjectMainFile*>(this->parent()), SLOT(moveDownImage(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestDeleteMeasuredData(QModelIndex)), dynamic_cast<ProjectMainFile*>(this->parent()), SLOT(deleteMeasuredData(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestMoveUpMeasuredData(QModelIndex)), dynamic_cast<ProjectMainFile*>(this->parent()), SLOT(moveUpMeasuredData(QModelIndex)));
	connect(parent->m_objectBrowser->view(), SIGNAL(requestMoveDownMeasuredData(QModelIndex)), dynamic_cast<ProjectMainFile*>(this->parent()), SLOT(moveDownMeasuredData(QModelIndex)));

}

PreProcessorWindowProjectDataItem::~PreProcessorWindowProjectDataItem()
{
	// explicitly delete the datamodel, so that it is sure
	// datamodel is destructed earlier than graphicsview.
	PreProcessorWindow* parent = m_preProcessorWindow;
	if (parent->isVisible()) {
		parent->m_isLastHiding = true;
	}
	if (parent->m_dataModel != nullptr) {
		delete parent->m_dataModel;
		parent->m_dataModel = nullptr;
	}
	parent->m_projectDataItem = nullptr;
	parent->m_graphicsView->setActiveDataItem(nullptr);
}


void PreProcessorWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	PreProcessorWindow* parent = m_preProcessorWindow;
	/// load background color
	QColor col = ProjectDataItem::loadBackgroundColor(node, QColor(Qt::white));
	m_preProcessorWindow->setBackgroundColor(col);
	/// load Window settings
	ProjectDataItem::loadWindowGeometry(m_preProcessorWindow->parentWidget(), node);
	/// load Camera settings
	QDomNode cameraNode = iRIC::getChildNode(node, "Camera");
	if (! cameraNode.isNull()) {
		vtkCamera* camera = m_preProcessorWindow->m_dataModel->graphicsView()->mainRenderer()->GetActiveCamera();
		ProjectDataItem::loadCamera(camera, cameraNode);
	}
	/// load Object Browser settings
	QDomNode modelNode = iRIC::getChildNode(node, "DataModel");
	if (! modelNode.isNull()) {
		parent->m_dataModel->loadFromProjectMainFile(modelNode);
		parent->m_dataModel->reflectExpandState(parent->m_objectBrowser->view());
	}
}

void PreProcessorWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	/// save background color
	ProjectDataItem::writeBackgroundColor(m_preProcessorWindow->backgroundColor(), writer);

	/// save Window geometry
	ProjectDataItem::writeWindowGeometry(m_preProcessorWindow->parentWidget(), writer);

	/// save Camera settings
	writer.writeStartElement("Camera");
	vtkCamera* camera = m_preProcessorWindow->m_dataModel->graphicsView()->mainRenderer()->GetActiveCamera();
	ProjectDataItem::saveCamera(camera, writer);
	writer.writeEndElement();

	/// save Object Browser setings
	writer.writeStartElement("DataModel");
	PreProcessorWindow* parent = m_preProcessorWindow;
	parent->m_dataModel->updateExpandState(parent->m_objectBrowser->view());
	parent->m_dataModel->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void PreProcessorWindowProjectDataItem::loadFromCgnsFile(const int fn)
{
	m_preProcessorWindow->m_dataModel->loadFromCgnsFile(fn);
}

void PreProcessorWindowProjectDataItem::saveToCgnsFile(const int fn)
{
	m_preProcessorWindow->m_dataModel->saveToCgnsFile(fn);
}

void PreProcessorWindowProjectDataItem::closeCgnsFile()
{
	m_preProcessorWindow->m_dataModel->closeCgnsFile();
}

QStringList PreProcessorWindowProjectDataItem::containedFiles()
{
	PreProcessorDataModel* model = dynamic_cast<PreProcessorDataModel*>(m_preProcessorWindow->m_dataModel);
	return model->m_rootDataItem->containedFiles();
}

bool PreProcessorWindowProjectDataItem::gridEdited()
{
	PreProcessorDataModel* model = dynamic_cast<PreProcessorDataModel*>(m_preProcessorWindow->m_dataModel);
	return model->gridEdited();
}

void PreProcessorWindowProjectDataItem::toggleGridEditFlag()
{
	PreProcessorDataModel* model = dynamic_cast<PreProcessorDataModel*>(m_preProcessorWindow->m_dataModel);
	model->toggleGridEditFlag();
}
