#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalargrouptopdataitem.h"
// #include "post2dbirdeyewindowcellscalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>

Post2dBirdEyeWindowZoneDataItem::Post2dBirdEyeWindowZoneDataItem(const std::string& zoneName, int zoneNumber, GraphicsWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {zoneName.c_str(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_scalarGroupTopDataItem {nullptr},
	// m_cellScalarGroupTopDataItem {nullptr},
	m_zoneName (zoneName),
	m_zoneNumber {zoneNumber}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dataContainer();
	m_shapeDataItem = new Post2dBirdEyeWindowGridShapeDataItem(this);

	if (cont->scalarValueExists()) {
		m_scalarGroupTopDataItem = new Post2dBirdEyeWindowNodeScalarGroupTopDataItem(this);
	}

	/*
	if (cont->cellScalarValueExists()) {
		m_cellScalarGroupTopDataItem = new Post2dBirdEyeWindowCellScalarGroupTopDataItem(this);
	}
	*/

	m_childItems.push_back(m_shapeDataItem);
	if (cont->scalarValueExists()) {
		m_childItems.push_back(m_scalarGroupTopDataItem);
	}
	/*
	if (cont->cellScalarValueExists()) {
		m_childItems.push_back(m_cellScalarGroupTopDataItem);
	}
	*/
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
	/*
	QDomNode cellScalarNode = iRIC::getChildNode(node, "ScalarCellCenter");
	if (!cellScalarNode.isNull() && m_cellScalarGroupTopDataItem != nullptr) {
		m_cellScalarGroupTopDataItem->loadFromProjectMainFile(cellScalarNode);
	}
	*/
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
	/*
	if (m_cellScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarCellCenter");
		m_cellScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	*/
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

PostZoneDataContainer* Post2dBirdEyeWindowZoneDataItem::dataContainer()
{
	return postSolutionInfo()->zoneContainer2D(m_zoneName);
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
	time.start();
	m_shapeDataItem->update();
	qDebug("Grid shape: %d", time.elapsed());

	if (m_scalarGroupTopDataItem != nullptr) {
		time.restart();
		m_scalarGroupTopDataItem->update();
		qDebug("Contour shape: %d", time.elapsed());
	}
	/*
	if (m_cellScalarGroupTopDataItem != nullptr) {
		time.restart();
		m_cellScalarGroupTopDataItem->update();
		qDebug("Cell Contour shape: %d", time.elapsed());
	}
	*/
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

/*
Post2dBirdEyeWindowCellScalarGroupTopDataItem* Post2dBirdEyeWindowZoneDataItem::cellScalarGroupTopDataItem() const
{
	return m_cellScalarGroupTopDataItem;
}
*/

void Post2dBirdEyeWindowZoneDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(m_childItems.size() + 1);
}

void Post2dBirdEyeWindowZoneDataItem::assignActorZValues(const ZDepthRange& range)
{
	int itemCount = m_childItems.size();
	int gapCount = itemCount - 1;
	float gapRate = .1; // the rate of gap width againt data width.

	double divWidth = range.width() / (itemCount + gapCount * gapRate);

	ZDepthRange r;
	double max, min;
	PostZoneDataContainer* cont = dataContainer();

	// the first is grid shape.
	max = range.max();
	min = max - divWidth;
	r = m_shapeDataItem->zDepthRange();
	r.setRange(min, max);
	m_shapeDataItem->setZDepthRange(r);

	// Contour
	if (cont->scalarValueExists()) {
		max = min - divWidth * gapRate;
		min = max - divWidth;
		r = m_scalarGroupTopDataItem->zDepthRange();
		r.setRange(min, max);
		m_scalarGroupTopDataItem->setZDepthRange(r);
	}
}
