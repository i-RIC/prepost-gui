#include "post2dbirdeyewindowcellscalargrouptopdataitem.h"
#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include "private/post2dbirdeyewindowzonedataitem_impl.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>

Post2dBirdEyeWindowZoneDataItem::Impl::Impl(const std::string& zoneName) :
	m_shapeDataItem {nullptr},
	m_scalarGroupTopDataItem {nullptr},
	m_cellScalarGroupTopDataItem {nullptr},
	m_zoneName (zoneName)
{}

Post2dBirdEyeWindowZoneDataItem::Impl::~Impl()
{}

Post2dBirdEyeWindowZoneDataItem::Post2dBirdEyeWindowZoneDataItem(const std::string& zoneName, GraphicsWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {zoneName.c_str(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	impl {new Impl {zoneName}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto grid = v4DataContainer()->gridData();
	impl->m_shapeDataItem = new Post2dBirdEyeWindowGridShapeDataItem(this);

	if (grid->scalarValueExists(v4SolutionGrid::Position::Node)) {
		impl->m_scalarGroupTopDataItem = new Post2dBirdEyeWindowNodeScalarGroupTopDataItem(this);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::CellCenter)) {
		impl->m_cellScalarGroupTopDataItem = new Post2dBirdEyeWindowCellScalarGroupTopDataItem(this);
	}

	addChildItem(impl->m_shapeDataItem);
	addChildItem(impl->m_scalarGroupTopDataItem);
	addChildItem(impl->m_cellScalarGroupTopDataItem);
}

void Post2dBirdEyeWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		impl->m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	// old
	QDomNode scalarGroupNode = iRIC::getChildNode(node, "ScalarGroup");
	if (! scalarGroupNode.isNull() && impl->m_scalarGroupTopDataItem != nullptr) {
		impl->m_scalarGroupTopDataItem->loadFromProjectMainFile(scalarGroupNode);
	}
	// new
	QDomNode contourGroupNode = iRIC::getChildNode(node, "Contours");
	if (! contourGroupNode.isNull() && impl->m_scalarGroupTopDataItem != nullptr) {
		impl->m_scalarGroupTopDataItem->loadFromProjectMainFile(contourGroupNode);
	}
	QDomNode cellScalarNode = iRIC::getChildNode(node, "ScalarCellCenter");
	if (!cellScalarNode.isNull() && impl->m_cellScalarGroupTopDataItem != nullptr) {
		impl->m_cellScalarGroupTopDataItem->loadFromProjectMainFile(cellScalarNode);
	}
}

void Post2dBirdEyeWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", impl->m_zoneName.c_str());
	writer.writeStartElement("Shape");
	impl->m_shapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (impl->m_scalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("Contours");
		impl->m_scalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_cellScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarCellCenter");
		impl->m_cellScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dBirdEyeWindowZoneDataItem::addCustomMenuItems(QMenu* /*menu*/)
{}

void Post2dBirdEyeWindowZoneDataItem::informSelection(VTKGraphicsView* v)
{
	impl->m_shapeDataItem->informSelection(v);
}

void Post2dBirdEyeWindowZoneDataItem::informDeselection(VTKGraphicsView* v)
{
	impl->m_shapeDataItem->informDeselection(v);
}

v4PostZoneDataContainer* Post2dBirdEyeWindowZoneDataItem::v4DataContainer()
{
	return postSolutionInfo()->v4ZoneContainer2D(impl->m_zoneName);
}

std::string Post2dBirdEyeWindowZoneDataItem::zoneName() const
{
	return impl->m_zoneName;
}

void Post2dBirdEyeWindowZoneDataItem::update()
{
	impl->m_shapeDataItem->update();

	if (impl->m_scalarGroupTopDataItem != nullptr) {
		impl->m_scalarGroupTopDataItem->update();
	}
	if (impl->m_cellScalarGroupTopDataItem != nullptr) {
		impl->m_cellScalarGroupTopDataItem->update();
	}
}

Post2dBirdEyeWindowGridTypeDataItem* Post2dBirdEyeWindowZoneDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*> (parent());
}

Post2dBirdEyeWindowGridShapeDataItem* Post2dBirdEyeWindowZoneDataItem::gridShapeDataItem() const
{
	return impl->m_shapeDataItem;
}

Post2dBirdEyeWindowNodeScalarGroupTopDataItem* Post2dBirdEyeWindowZoneDataItem::scalarGroupTopDataItem() const
{
	return impl->m_scalarGroupTopDataItem;
}

Post2dBirdEyeWindowCellScalarGroupTopDataItem* Post2dBirdEyeWindowZoneDataItem::cellScalarGroupTopDataItem() const
{
	return impl->m_cellScalarGroupTopDataItem;
}
