#include "../post2dwindowgraphicsview.h"
#include "post2dwindowmeasureddatatopdataitem.h"
#include "post2dwindowrootdataitem.h"

#include <dataitem/measureddata/measureddatafiledataitem.h>
#include <dataitem/measureddata/measureddatapointgroupdataitem.h>
#include <dataitem/measureddata/measureddatavectorgroupdataitem.h>
#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemmultiselectingdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkColorTransferFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

Post2dWindowMeasuredDataTopDataItem::Post2dWindowMeasuredDataTopDataItem(GraphicsWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Measured Values"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_importAction {new QAction(QIcon(":/libs/guibase/images/iconImport.svg"), Post2dWindowMeasuredDataTopDataItem::tr("&Import..."), this)},
	m_deleteSelectedAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), Post2dWindowMeasuredDataTopDataItem::tr("Delete &Selected..."), this)},
	m_deleteAllAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), Post2dWindowMeasuredDataTopDataItem::tr("Delete &All..."), this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	m_standardItem->setData(QVariant("MEASUREDDATAS"), Qt::UserRole + 10);
	setSubPath("measureddata");

	setupActors();

	connect(m_importAction, SIGNAL(triggered()), iricMainWindow(), SLOT(importMeasuredData()));
	connect(m_deleteSelectedAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));
	connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataAdded()), this, SLOT(addChildItem()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataDeleted(int)), this, SLOT(deleteChildItem(int)));
	connect(dynamic_cast<Post2dWindowRootDataItem*>(this->parent()), SIGNAL(standardModelSetuped()), this, SLOT(setupChildItem()));
	connect(this, SIGNAL(selectMeasuredData(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
}

Post2dWindowMeasuredDataTopDataItem::~Post2dWindowMeasuredDataTopDataItem()
{}

void Post2dWindowMeasuredDataTopDataItem::setupChildItem()
{
	if (projectData()->mainfile()->measuredDatas().size() == 0) {return;}
	const auto& measuredDatas = projectData()->mainfile()->measuredDatas();
	for (auto data : measuredDatas) {
		auto fItem = new MeasuredDataFileDataItem(data, this);
		// there is no need to make the standard item top.
		m_childItems.push_back(fItem);
	}
	updateItemMap();
	updateZDepthRange();

	dataModel()->graphicsView()->ResetCameraClippingRange();
	setModified();
}

void Post2dWindowMeasuredDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	std::unordered_map<int, MeasuredDataFileDataItem*> items;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<MeasuredDataFileDataItem*> (child);
		items.insert({item->measuredData()->index(), item});
	}

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		int index = child.attribute("index").toInt();
		auto it = items.find(index);
		if (it != items.end()) {
			auto item = it->second;
			item->loadFromProjectMainFile(child);
		}
	}
}

void Post2dWindowMeasuredDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto child : m_childItems) {
		writer.writeStartElement("MeasuredDataFile");
		child->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

const QList<MeasuredDataFileDataItem*> Post2dWindowMeasuredDataTopDataItem::fileDataItems() const
{
	QList<MeasuredDataFileDataItem*> ret;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<MeasuredDataFileDataItem*>(child);
		ret.append(item);
	}
	return ret;
}

void Post2dWindowMeasuredDataTopDataItem::setupActors()
{}

void Post2dWindowMeasuredDataTopDataItem::updateActorSettings()
{}

void Post2dWindowMeasuredDataTopDataItem::addChildItem()
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

	dataModel()->graphicsView()->ResetCameraClippingRange();
	dataModel()->graphicsView()->cameraFit();
	emit selectMeasuredData(fItem->standardItem()->index());
}

void Post2dWindowMeasuredDataTopDataItem::deleteChildItem(int index)
{
	auto it = m_childItems.begin();
	delete *(it + index);
	dynamic_cast<Post2dWindowRootDataItem*>(parent())->updateItemMap();
	renderGraphicsView();
}

void Post2dWindowMeasuredDataTopDataItem::deleteSelected()
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

void Post2dWindowMeasuredDataTopDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all measured data?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	while (m_childItems.size() > 0) {
		GraphicsWindowDataItem* item = *(m_childItems.begin());
		projectData()->mainfile()->deleteMeasuredData(item->standardItem()->index());
	}
}

void Post2dWindowMeasuredDataTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_importAction);
	menu->addSeparator();
	menu->addAction(m_deleteSelectedAction);
	menu->addAction(m_deleteAllAction);
}
