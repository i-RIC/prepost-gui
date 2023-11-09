#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridattributecellgroupdataitem.h"
#include "post2dwindowgridattributeiedgegroupdataitem.h"
#include "post2dwindowgridattributejedgegroupdataitem.h"
#include "post2dwindowgridattributenodegroupdataitem.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowinputgriddataitem_impl.h"

#include <guicore/grid/v4grid2d.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <misc/xmlsupport.h>

Post2dWindowInputGridDataItem::Post2dWindowInputGridDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowGridDataItemI {tr("Input Grid"), parent},
	impl {new Impl {}}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	auto geoDataTop = zoneDataItem()->gridTypeDataItem()->geoDataItem();
	auto grid = inputGrid();
	for (auto att : grid->attributes()) {
		auto gItem = geoDataTop->preGeoDataTopDataItem()->groupDataItem(att->name());
		att->setDimensions(gItem->dimensions());
	}

	impl->m_gridShapeDataItem = new Post2dWindowGridShapeDataItem(this);
	impl->m_nodeGroupDataItem = new Post2dWindowGridAttributeNodeGroupDataItem(this);

	impl->m_cellGroupDataItem = new Post2dWindowGridAttributeCellGroupDataItem(this);
	impl->m_cellGroupDataItem->setupChildren();

	impl->m_iEdgeGroupDataItem = new Post2dWindowGridAttributeIEdgeGroupDataItem(this);
	impl->m_iEdgeGroupDataItem->setupChildren();

	impl->m_jEdgeGroupDataItem = new Post2dWindowGridAttributeJEdgeGroupDataItem(this);
	impl->m_jEdgeGroupDataItem->setupChildren();

	addChildItem(impl->m_gridShapeDataItem);
	addChildItem(impl->m_nodeGroupDataItem);
	addChildItem(impl->m_cellGroupDataItem);
	addChildItem(impl->m_iEdgeGroupDataItem);
	addChildItem(impl->m_jEdgeGroupDataItem);

	if (impl->m_nodeGroupDataItem->childItems().size() == 0) {
		m_standardItem->takeRow(impl->m_nodeGroupDataItem->standardItem()->row());
	}
	if (impl->m_cellGroupDataItem->childItems().size() == 0) {
		m_standardItem->takeRow(impl->m_cellGroupDataItem->standardItem()->row());
	}
	if (impl->m_iEdgeGroupDataItem->childItems().size() == 0) {
		m_standardItem->takeRow(impl->m_iEdgeGroupDataItem->standardItem()->row());
	}
	if (impl->m_jEdgeGroupDataItem->childItems().size() == 0) {
		m_standardItem->takeRow(impl->m_jEdgeGroupDataItem->standardItem()->row());
	}
}

Post2dWindowInputGridDataItem::~Post2dWindowInputGridDataItem()
{}

v4InputGrid* Post2dWindowInputGridDataItem::inputGrid() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	return cont->inputGridData();
}

v4Grid2d* Post2dWindowInputGridDataItem::grid() const
{
	auto iGrid = inputGrid();
	if (iGrid == nullptr) {return nullptr;}

	return dynamic_cast<v4Grid2d*> (iGrid->grid());
}

Post2dWindowZoneDataItem* Post2dWindowInputGridDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

Post2dWindowGridShapeDataItem* Post2dWindowInputGridDataItem::gridShapeDataItem() const
{
	return impl->m_gridShapeDataItem;
}

void Post2dWindowInputGridDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {impl->m_gridShapeDataItem->loadFromProjectMainFile(shapeNode);}

	QDomNode nodeNode = iRIC::getChildNode(node, "NodeAttributes");
	if (! nodeNode.isNull()) {impl->m_nodeGroupDataItem->loadFromProjectMainFile(nodeNode);}

	QDomNode cellNode = iRIC::getChildNode(node, "CellAttributes");
	if (! cellNode.isNull()) {impl->m_cellGroupDataItem->loadFromProjectMainFile(cellNode);}

	QDomNode iEdgeNode = iRIC::getChildNode(node, "IEdgeAttributes");
	if (! iEdgeNode.isNull()) {impl->m_iEdgeGroupDataItem->loadFromProjectMainFile(iEdgeNode);}

	QDomNode jEdgeNode = iRIC::getChildNode(node, "JEdgeAttributes");
	if (! jEdgeNode.isNull()) {impl->m_jEdgeGroupDataItem->loadFromProjectMainFile(jEdgeNode);}
}

void Post2dWindowInputGridDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("Shape");
	impl->m_gridShapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("NodeAttributes");
	impl->m_nodeGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("CellAttributes");
	impl->m_cellGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("IEdgeAttributes");
	impl->m_iEdgeGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("JEdgeAttributes");
	impl->m_jEdgeGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post2dWindowInputGridDataItem::update()
{
	impl->m_gridShapeDataItem->update();
	impl->m_nodeGroupDataItem->update();
	impl->m_cellGroupDataItem->update();
	impl->m_iEdgeGroupDataItem->update();
	impl->m_jEdgeGroupDataItem->update();
}

bool Post2dWindowInputGridDataItem::colorBarShouldBeVisible(const std::string& name) const
{
	if (impl->m_nodeGroupDataItem->isChecked()) {
		return impl->m_nodeGroupDataItem->colorBarShouldBeVisible(name);
	} else if (impl->m_cellGroupDataItem->isChecked()) {
		return impl->m_cellGroupDataItem->colorBarShouldBeVisible(name);
	} else if (impl->m_iEdgeGroupDataItem->isChecked()) {
		return impl->m_iEdgeGroupDataItem->colorBarShouldBeVisible(name);
	} else if (impl->m_jEdgeGroupDataItem->isChecked()) {
		return impl->m_jEdgeGroupDataItem->colorBarShouldBeVisible(name);
	}
	return false;
}
