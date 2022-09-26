#include "distancemeasuredataitem.h"
#include "distancemeasuregroupdataitem.h"
#include "private/distancemeasuregroupdataitem_impl.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemmultiselectingdialog.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <misc/iricundostack.h>

#include <QDomNode>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

DistanceMeasureGroupDataItem::Impl::Impl(DistanceMeasureGroupDataItem *parent) :
	m_addAction {new QAction {DistanceMeasureGroupDataItem::tr("&Add Measure..."), parent}},
	m_deleteSelectedAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), DistanceMeasureGroupDataItem::tr("Delete &Selected..."), parent)},
	m_deleteAllAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.svg"), DistanceMeasureGroupDataItem::tr("Delete &All..."), parent)}
{}

DistanceMeasureGroupDataItem::DistanceMeasureGroupDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("Distance Measures"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	impl {new Impl {this}}
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	connect(impl->m_addAction, SIGNAL(triggered()), this, SLOT(addMeasure()));
	connect(impl->m_deleteSelectedAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));
	connect(impl->m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));

	// set up the first item.
	addMeasure();
}

DistanceMeasureGroupDataItem::~DistanceMeasureGroupDataItem()
{
	delete impl;
}

void DistanceMeasureGroupDataItem::addMeasure()
{
	int count = static_cast<int>(m_childItems.size());
	QString defaultName = tr("Measure%1").arg(count + 1);
	auto child = new DistanceMeasureDataItem(defaultName, this);
	m_childItems.push_back(child);
	updateItemMap();
	updateZDepthRange();
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	if (view != 0) {
		view->select(child->standardItem()->index());
	}

	// This operation is not undo-able.
	iRICUndoStack::instance().clear();
}

void DistanceMeasureGroupDataItem::deleteSelected()
{
	if (m_childItems.size() == 0) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("There is no distance measure."), QMessageBox::Ok);
		return;
	}

	auto items = m_childItems;

	std::vector<QString> names;
	for (auto item : items) {
		names.push_back(item->standardItem()->text());
	}

	ItemMultiSelectingDialog dialog(mainWindow());
	dialog.setWindowTitle(tr("Delete selected distance measures"));
	dialog.setItems(names);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto settings = dialog.selectSettings();
	for (int i = 0; i < settings.size(); ++i) {
		if (settings.at(i)) {
			// delete the item
			delete items.at(i);
		}
	}
}

void DistanceMeasureGroupDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all distance measures?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	auto items = m_childItems;
	for (auto item : items) {
		delete item;
	}
}

void DistanceMeasureGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void DistanceMeasureGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_addAction);
	menu->addSeparator();
	menu->addAction(impl->m_deleteSelectedAction);
	menu->addAction(impl->m_deleteAllAction);
}

void DistanceMeasureGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomNode childNode = children.at(i);
		auto item = new DistanceMeasureDataItem("", this);
		item->loadFromProjectMainFile(childNode);
		m_childItems.push_back(item);
	}
}

void DistanceMeasureGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (GraphicsWindowDataItem* item : m_childItems) {
		writer.writeStartElement("DistanceMeasure");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
