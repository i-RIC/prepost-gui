#include "../misc/preprocessorscalarbarlegendboxsettingdialog.h"
#include "../preprocessorgraphicsview.h"
#include "preprocessormeasureddatatopdataitem.h"
#include "preprocessorrootdataitem.h"

#include <dataitem/measureddata/measureddatafiledataitem.h>
#include <dataitem/measureddata/measureddatapointgroupdataitem.h>
#include <dataitem/measureddata/measureddatavectorgroupdataitem.h>
#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemmultiselectingdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QDomNode>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkColorTransferFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PreProcessorMeasuredDataTopDataItem::PreProcessorMeasuredDataTopDataItem(GraphicsWindowDataItem* parent) :
	PreProcessorDataItem {tr("Measured Values"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_importAction {new QAction(QIcon(":/libs/guibase/images/iconImport.svg"), PreProcessorMeasuredDataTopDataItem::tr("&Import..."), this)},
	m_deleteSelectedAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), PreProcessorMeasuredDataTopDataItem::tr("Delete &Selected..."), this)},
	m_deleteAllAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), PreProcessorMeasuredDataTopDataItem::tr("Delete &All..."), this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath("measureddata");

	setIsCommandExecuting(true);
	m_standardItem->setData(QVariant("MEASUREDDATAS"), Qt::UserRole + 10);
	setIsCommandExecuting(false);

	setupActors();

	connect(m_importAction, SIGNAL(triggered()), iricMainWindow(), SLOT(importMeasuredData()));
	connect(m_deleteSelectedAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));
	connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataAdded()), this, SLOT(addChildItem()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataDeleted(int)), this, SLOT(deleteChildItem(int)));
	connect(this, SIGNAL(selectMeasuredData(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
}

PreProcessorMeasuredDataTopDataItem::~PreProcessorMeasuredDataTopDataItem()
{

}

void PreProcessorMeasuredDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	const auto& mdlist = projectData()->mainfile()->measuredDatas();
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		int index = child.attribute("index").toInt();
		for (MeasuredData* md : mdlist) {
			if (md->index() == index) {
				MeasuredDataFileDataItem* fitem = new MeasuredDataFileDataItem(md, this);
				fitem->loadFromProjectMainFile(child);
				m_childItems.push_back(fitem);
			}
		}
	}
}

void PreProcessorMeasuredDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("MeasuredDataFile");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

const QList<MeasuredDataFileDataItem*> PreProcessorMeasuredDataTopDataItem::fileDataItems() const
{
	QList<MeasuredDataFileDataItem*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		MeasuredDataFileDataItem* item = dynamic_cast<MeasuredDataFileDataItem*>(*it);
		ret.append(item);
	}
	return ret;
}

void PreProcessorMeasuredDataTopDataItem::setupActors()
{

}

void PreProcessorMeasuredDataTopDataItem::updateActorSettings()
{

}

void PreProcessorMeasuredDataTopDataItem::addChildItem()
{
	MeasuredData* md = *(projectData()->mainfile()->measuredDatas().rbegin());
	MeasuredDataFileDataItem* fItem = new MeasuredDataFileDataItem(md, this);
	m_childItems.push_back(fItem);

	updateItemMap();
	updateZDepthRange();

	ObjectBrowserView* oview = dataModel()->objectBrowserView();
	oview->select(fItem->standardItem()->index());
	oview->expand(fItem->standardItem()->index());
	oview->expand(fItem->pointGroupDataItem()->standardItem()->index());
	oview->expand(fItem->vectorGroupDataItem()->standardItem()->index());
	dataModel()->graphicsView()->cameraFit();
	emit selectMeasuredData(fItem->standardItem()->index());
}

void PreProcessorMeasuredDataTopDataItem::deleteChildItem(int index)
{
	auto it = m_childItems.begin();
	delete *(it + index);
	dynamic_cast<PreProcessorRootDataItem*>(parent())->updateItemMap();
	renderGraphicsView();
}

void PreProcessorMeasuredDataTopDataItem::deleteSelected()
{
	if (m_childItems.size() == 0) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("There is no measured data."), QMessageBox::Ok);
		return;
	}

	auto items = m_childItems;

	std::vector<QString> names;
	for (auto item : items) {
		names.push_back(item->standardItem()->text());
	}

	ItemMultiSelectingDialog dialog(mainWindow());
	dialog.setWindowTitle(tr("Delete selected measured data"));
	dialog.setItems(names);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto settings = dialog.selectSettings();
	auto mainfile = projectData()->mainfile();
	for (int i = 0; i < settings.size(); ++i) {
		if (settings.at(i)) {
			// delete the item
			mainfile->deleteMeasuredData(items.at(i)->standardItem()->index());
		}
	}
}

void PreProcessorMeasuredDataTopDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all measured data?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	while (m_childItems.size() > 0) {
		GraphicsWindowDataItem* item = *(m_childItems.begin());
		projectData()->mainfile()->deleteMeasuredData(item->standardItem()->index());
	}
}

void PreProcessorMeasuredDataTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_importAction);
	menu->addSeparator();
	menu->addAction(m_deleteSelectedAction);
	menu->addAction(m_deleteAllAction);
}
