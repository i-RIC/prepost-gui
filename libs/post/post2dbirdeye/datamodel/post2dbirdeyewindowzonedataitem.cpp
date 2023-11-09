#include "post2dbirdeyewindowcellscalargrouptopdataitem.h"
#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>

Post2dBirdEyeWindowZoneDataItem::Post2dBirdEyeWindowZoneDataItem(const std::string& zoneName, int zoneNumber, GraphicsWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {zoneName.c_str(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_scalarGroupTopDataItem {nullptr},
	m_cellScalarGroupTopDataItem {nullptr},
	m_zoneName (zoneName),
	m_zoneNumber {zoneNumber}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto grid = v4DataContainer()->gridData();
	m_shapeDataItem = new Post2dBirdEyeWindowGridShapeDataItem(this);

	if (grid->scalarValueExists(v4SolutionGrid::Position::Node)) {
		m_scalarGroupTopDataItem = new Post2dBirdEyeWindowNodeScalarGroupTopDataItem(this);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::CellCenter)) {
		m_cellScalarGroupTopDataItem = new Post2dBirdEyeWindowCellScalarGroupTopDataItem(this);
	}

	m_childItems.push_back(m_shapeDataItem);
	if (m_scalarGroupTopDataItem != nullptr) {
		m_childItems.push_back(m_scalarGroupTopDataItem);
	}
	if (m_cellScalarGroupTopDataItem != nullptr) {
		m_childItems.push_back(m_cellScalarGroupTopDataItem);
	}
}

void Post2dBirdEyeWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	// old
	QDomNode scalarGroupNode = iRIC::getChildNode(node, "ScalarGroup");
	if (! scalarGroupNode.isNull() && m_scalarGroupTopDataItem != nullptr) {
		m_scalarGroupTopDataItem->loadFromProjectMainFile(scalarGroupNode);
	}
	// new
	QDomNode contourGroupNode = iRIC::getChildNode(node, "Contours");
	if (! contourGroupNode.isNull() && m_scalarGroupTopDataItem != nullptr) {
		m_scalarGroupTopDataItem->loadFromProjectMainFile(contourGroupNode);
	}
	QDomNode cellScalarNode = iRIC::getChildNode(node, "ScalarCellCenter");
	if (!cellScalarNode.isNull() && m_cellScalarGroupTopDataItem != nullptr) {
		m_cellScalarGroupTopDataItem->loadFromProjectMainFile(cellScalarNode);
	}
}

void Post2dBirdEyeWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_zoneName.c_str());
	writer.writeStartElement("Shape");
	m_shapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (m_scalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("Contours");
		m_scalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_cellScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarCellCenter");
		m_cellScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dBirdEyeWindowZoneDataItem::addCustomMenuItems(QMenu* /*menu*/)
{}

void Post2dBirdEyeWindowZoneDataItem::informSelection(VTKGraphicsView* v)
{
	m_shapeDataItem->informSelection(v);
}

void Post2dBirdEyeWindowZoneDataItem::informDeselection(VTKGraphicsView* v)
{
	m_shapeDataItem->informDeselection(v);
}

v4PostZoneDataContainer* Post2dBirdEyeWindowZoneDataItem::v4DataContainer()
{
	return postSolutionInfo()->v4ZoneContainer2D(m_zoneName);
}

int Post2dBirdEyeWindowZoneDataItem::zoneNumber() const
{
	return m_zoneNumber;
}

std::string Post2dBirdEyeWindowZoneDataItem::zoneName() const
{
	return m_zoneName;
}

void Post2dBirdEyeWindowZoneDataItem::update()
{
	QTime time;
	m_shapeDataItem->update();

	if (m_scalarGroupTopDataItem != nullptr) {
		m_scalarGroupTopDataItem->update();
	}
	if (m_cellScalarGroupTopDataItem != nullptr) {
		m_cellScalarGroupTopDataItem->update();
	}
}

Post2dBirdEyeWindowGridTypeDataItem* Post2dBirdEyeWindowZoneDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*> (parent());
}

Post2dBirdEyeWindowGridShapeDataItem* Post2dBirdEyeWindowZoneDataItem::gridShapeDataItem() const
{
	return m_shapeDataItem;
}

Post2dBirdEyeWindowNodeScalarGroupTopDataItem* Post2dBirdEyeWindowZoneDataItem::scalarGroupTopDataItem() const
{
	return m_scalarGroupTopDataItem;
}

Post2dBirdEyeWindowCellScalarGroupTopDataItem* Post2dBirdEyeWindowZoneDataItem::cellScalarGroupTopDataItem() const
{
	return m_cellScalarGroupTopDataItem;
}
