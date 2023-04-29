#include "../gridbirdeyeobjectbrowser.h"
#include "../gridbirdeyewindow.h"
#include "../gridbirdeyewindowdatamodel.h"
#include "gridbirdeyewindowcellscalargrouptopdataitem.h"
#include "gridbirdeyewindowgridshapedataitem.h"
#include "gridbirdeyewindownodescalargrouptopdataitem.h"
#include "gridbirdeyewindowrootdataitem.h"
#include "gridbirdeyewindowzonedataitem.h"

#include <dataitem/axis3d/axis3ddataitem.h>
#include <misc/xmlsupport.h>

#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

GridBirdEyeWindowRootDataItem::GridBirdEyeWindowRootDataItem(GridBirdEyeWindow* window, ProjectDataItem* parent) :
	GraphicsWindowRootDataItem {window, parent},
	m_zoneDataItem {new GridBirdEyeWindowZoneDataItem {this}},
	m_axesDataItem {new Axis3dDataItem {this}}
{
	m_childItems.push_back(m_zoneDataItem);
	m_childItems.push_back(m_axesDataItem);

	updateZDepthRangeItemCount();
	// update item map initially.
	updateItemMap();
}

GridBirdEyeWindowRootDataItem::~GridBirdEyeWindowRootDataItem()
{
	delete m_zoneDataItem;
	delete m_axesDataItem;
}

void GridBirdEyeWindowRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();

	m_zoneDataItem->standardItem()->takeChild(m_zoneDataItem->gridShapeDataItem()->standardItem()->row());
	model->appendRow(m_zoneDataItem->gridShapeDataItem()->standardItem());

	if (m_zoneDataItem->scalarGroupTopDataItem() != nullptr) {
		m_zoneDataItem->standardItem()->takeChild(m_zoneDataItem->scalarGroupTopDataItem()->standardItem()->row());
		model->appendRow(m_zoneDataItem->scalarGroupTopDataItem()->standardItem());
	}
	if (m_zoneDataItem->cellScalarGroupTopDataItem() != nullptr) {
		m_zoneDataItem->standardItem()->takeChild(m_zoneDataItem->cellScalarGroupTopDataItem()->standardItem()->row());
		model->appendRow(m_zoneDataItem->cellScalarGroupTopDataItem()->standardItem());
	}
	model->appendRow(m_axesDataItem->standardItem());
}

void GridBirdEyeWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode zoneNode = iRIC::getChildNode(node, "Zone");
	if (! zoneNode.isNull()) {m_zoneDataItem->loadFromProjectMainFile(zoneNode);}

	updateItemMap();
}
void GridBirdEyeWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("Zone");
	m_zoneDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void GridBirdEyeWindowRootDataItem::update()
{
	m_zoneDataItem->update();
	renderGraphicsView();
}

GridBirdEyeWindowZoneDataItem* GridBirdEyeWindowRootDataItem::zoneDataItem() const
{
	return m_zoneDataItem;
}
