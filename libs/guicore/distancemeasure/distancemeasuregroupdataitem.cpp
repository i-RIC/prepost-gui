#include "../datamodel/graphicswindowdatamodel.h"
#include "distancemeasuredataitem.h"
#include "distancemeasuregroupdataitem.h"

#include <guibase/objectbrowserview.h>
#include <misc/iricundostack.h>

#include <QDomNode>
#include <QMenu>

DistanceMeasureGroupDataItem::DistanceMeasureGroupDataItem(GraphicsWindowDataItem* parent)
	: GraphicsWindowDataItem(tr("Distance Measures"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_addAction = new QAction(tr("&Add Measure..."), this);

	connect(m_addAction, SIGNAL(triggered()), this, SLOT(addMeasure()));

	addMeasure();
}

void DistanceMeasureGroupDataItem::addMeasure()
{
	int count = m_childItems.count();
	QString defaultName = tr("Measure%1").arg(count + 1);
	DistanceMeasureDataItem* child = new DistanceMeasureDataItem(defaultName, this);
	m_childItems.push_back(child);
	updateItemMap();
	updateZDepthRange();
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	if (view != 0) {
		view->select(child->standardItem()->index());
	}

	iRICUndoStack::instance().clear();
}

void DistanceMeasureGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_addAction);
}

void DistanceMeasureGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomNode childNode = children.at(i);
		DistanceMeasureDataItem* item = new DistanceMeasureDataItem("", this);
		item->loadFromProjectMainFile(childNode);
		m_childItems.append(item);
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
