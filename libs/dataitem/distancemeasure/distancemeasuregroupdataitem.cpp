#include "distancemeasuredataitem.h"
#include "distancemeasuregroupdataitem.h"
#include "private/distancemeasuregroupdataitem_impl.h"

#include <guibase/objectbrowserview.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <misc/iricundostack.h>

#include <QDomNode>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

DistanceMeasureGroupDataItem::Impl::Impl(DistanceMeasureGroupDataItem *parent) :
	m_addAction {new QAction {DistanceMeasureGroupDataItem::tr("&Add Measure..."), parent}}
{}

DistanceMeasureGroupDataItem::DistanceMeasureGroupDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("Distance Measures"), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	impl {new Impl {this}}
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	connect(impl->m_addAction, SIGNAL(triggered()), this, SLOT(addMeasure()));

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

void DistanceMeasureGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void DistanceMeasureGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_addAction);
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
