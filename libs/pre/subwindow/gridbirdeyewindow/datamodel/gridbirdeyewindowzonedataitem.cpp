#include "../../../datamodel/preprocessorgriddataitem.h"
#include "../../../datamodel/preprocessorgridtypedataitem.h"
#include "gridbirdeyewindowcellscalargrouptopdataitem.h"
#include "gridbirdeyewindowgridshapedataitem.h"
#include "gridbirdeyewindownodescalargrouptopdataitem.h"
#include "gridbirdeyewindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>

GridBirdEyeWindowZoneDataItem::GridBirdEyeWindowZoneDataItem(GraphicsWindowDataItem* parent) :
	GridBirdEyeWindowDataItem {"Grid", QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_scalarGroupTopDataItem {nullptr},
	m_cellScalarGroupTopDataItem {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto gridType = gridTypeDataItem()->gridType();

	bool nodeAttributeExists = false;
	bool cellAttributeExists = false;

	for (auto att : gridType->gridAttributes()) {
		if (att->position() == SolverDefinitionGridAttribute::Position::Node) {
			nodeAttributeExists = true;
		}
		if (att->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
			cellAttributeExists = true;
		}
	}
	for (auto att : gridType->gridComplexAttributes()) {
		if (att->position() == SolverDefinitionGridAttribute::Position::Node) {
			nodeAttributeExists = true;
		}
		if (att->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
			cellAttributeExists = true;
		}
	}

	m_shapeDataItem = new GridBirdEyeWindowGridShapeDataItem(this);
	m_childItems.push_back(m_shapeDataItem);

	if (nodeAttributeExists) {
		m_scalarGroupTopDataItem = new GridBirdEyeWindowNodeScalarGroupTopDataItem(this);
		m_childItems.push_back(m_scalarGroupTopDataItem);
	}
	if (cellAttributeExists) {
		m_cellScalarGroupTopDataItem = new GridBirdEyeWindowCellScalarGroupTopDataItem(this);
		m_childItems.push_back(m_cellScalarGroupTopDataItem);
	}
}

void GridBirdEyeWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void GridBirdEyeWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
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

void GridBirdEyeWindowZoneDataItem::addCustomMenuItems(QMenu* /*menu*/)
{}

void GridBirdEyeWindowZoneDataItem::informSelection(VTKGraphicsView* v)
{
	m_shapeDataItem->informSelection(v);
}

void GridBirdEyeWindowZoneDataItem::informDeselection(VTKGraphicsView* v)
{
	m_shapeDataItem->informDeselection(v);
}

v4InputGrid* GridBirdEyeWindowZoneDataItem::grid()
{
	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());
	return model->gridDataItem()->grid();
}

void GridBirdEyeWindowZoneDataItem::update()
{
	m_shapeDataItem->update();

	if (m_scalarGroupTopDataItem != nullptr) {
		m_scalarGroupTopDataItem->update();
	}
	if (m_cellScalarGroupTopDataItem != nullptr) {
		m_cellScalarGroupTopDataItem->update();
	}
}

PreProcessorGridTypeDataItem* GridBirdEyeWindowZoneDataItem::gridTypeDataItem() const
{
	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());
	return model->gridTypeDataItem();
}

GridBirdEyeWindowGridShapeDataItem* GridBirdEyeWindowZoneDataItem::gridShapeDataItem() const
{
	return m_shapeDataItem;
}

GridBirdEyeWindowNodeScalarGroupTopDataItem* GridBirdEyeWindowZoneDataItem::scalarGroupTopDataItem() const
{
	return m_scalarGroupTopDataItem;
}

GridBirdEyeWindowCellScalarGroupTopDataItem* GridBirdEyeWindowZoneDataItem::cellScalarGroupTopDataItem() const
{
	return m_cellScalarGroupTopDataItem;
}
