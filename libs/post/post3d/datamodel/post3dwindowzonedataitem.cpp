#include "../post3dwindowdatamodel.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowgridshapedataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindownodescalargrouptopdataitem.h"
#include "post3dwindownodevectorarrowtopdataitem.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post3dwindowparticlegrouprootdataitem.h"
#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowstringresultdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QIcon>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

Post3dWindowZoneDataItem::Post3dWindowZoneDataItem(const std::string& zoneName, int zoneNumber, Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {zoneName.c_str(), QIcon(":/images/iconGrid.png"), parent},
	m_shapeDataItem {nullptr},
	m_contourGroupTopItem {nullptr},
	m_cellContourGroupTopItem {nullptr},
	m_scalarGroupDataItem {nullptr},
	m_arrowTopDataItem {nullptr},
	m_streamlineGroupDataItem {nullptr},
	m_particleGroupDataItem {nullptr},
	m_particlesDataItem {nullptr},
	m_particleGroupRootDataItem {nullptr},
	m_stringDataItem {nullptr},
	m_zoneName (zoneName),
	m_zoneNumber {zoneNumber}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_shapeDataItem = new Post3dWindowGridShapeDataItem(this);

	auto cont = v4DataContainer();
	auto grid = cont->gridData();
	if (grid->scalarValueExists(v4SolutionGrid::Position::Node)) {
		m_contourGroupTopItem = new Post3dWindowContourGroupTopDataItem(this);
		m_scalarGroupDataItem = new Post3dWindowNodeScalarGroupTopDataItem(this);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::CellCenter)) {
		m_cellContourGroupTopItem = new Post3dWindowCellContourGroupTopDataItem(this);
	}

	if (grid->vectorValueExists(v4SolutionGrid::Position::Node)) {
		m_arrowTopDataItem = new Post3dWindowNodeVectorArrowTopDataItem(this);
		m_streamlineGroupDataItem = new Post3dWindowNodeVectorStreamlineGroupStructuredDataItem(this);
		m_particleGroupDataItem = new Post3dWindowNodeVectorParticleGroupStructuredDataItem(this);
	}

	if (cont->particleData() != nullptr) {
		m_particlesDataItem = new Post3dWindowParticlesTopDataItem(this);
		m_particlesDataItem->setup();
	}
	if (cont->particleGroupMap().size() > 0) {
		m_particleGroupRootDataItem = new Post3dWindowParticleGroupRootDataItem(this);
	}

	m_stringDataItem = new Post3dWindowStringResultDataItem(this);

	m_childItems.push_back(m_shapeDataItem);

	if (grid->scalarValueExists(v4SolutionGrid::Position::Node)) {
		m_childItems.push_back(m_contourGroupTopItem);
		m_childItems.push_back(m_scalarGroupDataItem);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::CellCenter)) {
		m_childItems.push_back(m_cellContourGroupTopItem);
	}

	if (grid->vectorValueExists(v4SolutionGrid::Position::Node)) {
		m_childItems.push_back(m_arrowTopDataItem);
		m_childItems.push_back(m_streamlineGroupDataItem);
		m_childItems.push_back(m_particleGroupDataItem);
	}

	if (m_particlesDataItem != nullptr) {
		m_childItems.push_back(m_particlesDataItem);
	}
	if (m_particleGroupRootDataItem != nullptr) {
		m_childItems.push_back(m_particleGroupRootDataItem);
	}
	m_childItems.push_back(m_stringDataItem);
}

Post3dWindowZoneDataItem::~Post3dWindowZoneDataItem()
{
	delete m_stringDataItem;
}

int Post3dWindowZoneDataItem::zoneNumber() const
{
	return m_zoneNumber;
}

const std::string& Post3dWindowZoneDataItem::zoneName() const
{
	return m_zoneName;
}

Post3dWindowGridTypeDataItem* Post3dWindowZoneDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post3dWindowGridTypeDataItem*> (parent());
}

Post3dWindowGridShapeDataItem* Post3dWindowZoneDataItem::gridShapeDataItem() const
{
	return m_shapeDataItem;
}

Post3dWindowContourGroupTopDataItem* Post3dWindowZoneDataItem::contourGroupTopItem() const
{
	return m_contourGroupTopItem;
}

Post3dWindowCellContourGroupTopDataItem* Post3dWindowZoneDataItem::cellContourGroupTopItem() const
{
	return m_cellContourGroupTopItem;
}

Post3dWindowNodeScalarGroupTopDataItem* Post3dWindowZoneDataItem::scalarGroupDataItem() const
{
	return m_scalarGroupDataItem;
}

Post3dWindowNodeVectorArrowTopDataItem* Post3dWindowZoneDataItem::arrowTopDataItem() const
{
	return m_arrowTopDataItem;
}

Post3dWindowNodeVectorStreamlineGroupDataItem* Post3dWindowZoneDataItem::streamlineGroupDataItem() const
{
	return m_streamlineGroupDataItem;
}

Post3dWindowNodeVectorParticleGroupDataItem* Post3dWindowZoneDataItem::particleGroupDataItem() const
{
	return m_particleGroupDataItem;
}

Post3dWindowParticlesTopDataItem* Post3dWindowZoneDataItem::particlesDataItem() const
{
	return m_particlesDataItem;
}

Post3dWindowStringResultDataItem* Post3dWindowZoneDataItem::stringDataItem() const
{
	return m_stringDataItem;
}

void Post3dWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	QDomNode contoursNode = iRIC::getChildNode(node, "ScalarNodeV4");
	if (!contoursNode.isNull() && m_contourGroupTopItem != nullptr) {
		m_contourGroupTopItem->loadFromProjectMainFile(contoursNode);
	}
	QDomNode cellContoursNode = iRIC::getChildNode(node, "ScalarCellV4");
	if (!cellContoursNode.isNull() && m_cellContourGroupTopItem != nullptr) {
		m_cellContourGroupTopItem->loadFromProjectMainFile(cellContoursNode);
	}
	QDomNode isosurfacesNode = iRIC::getChildNode(node, "Isosurfaces");
	if (! isosurfacesNode.isNull() && m_scalarGroupDataItem != nullptr) {
		// multi-isosurfaces
		m_scalarGroupDataItem->loadFromProjectMainFile(isosurfacesNode);
	} else {
		// single-isosurface
		QDomNode scalarGroupNode = iRIC::getChildNode(node, "ScalarGroup");
		if (! scalarGroupNode.isNull() && m_scalarGroupDataItem != nullptr) {
			m_scalarGroupDataItem->loadFromProjectMainFile(scalarGroupNode);
		}
	}
	QDomNode arrowGroupTopNode = iRIC::getChildNode(node, "ArrowGroupV4");
	if (! arrowGroupTopNode.isNull() && m_arrowTopDataItem != nullptr) {
		m_arrowTopDataItem->loadFromProjectMainFile(arrowGroupTopNode);
	}
	QDomNode streamlineGroupNode = iRIC::getChildNode(node, "StreamlineGroup");
	if (! streamlineGroupNode.isNull() && m_streamlineGroupDataItem != nullptr) {
		m_streamlineGroupDataItem->loadFromProjectMainFile(streamlineGroupNode);
	}
	QDomNode particleGroupNode = iRIC::getChildNode(node, "ParticleGroup");
	if (! particleGroupNode.isNull() && m_particleGroupDataItem != nullptr) {
		m_particleGroupDataItem->loadFromProjectMainFile(particleGroupNode);
	}
	QDomNode particlesNode = iRIC::getChildNode(node, "SolverParticlesV4");
	if (! particlesNode.isNull() && m_particlesDataItem != nullptr) {
		m_particlesDataItem->loadFromProjectMainFile(particlesNode);
	}
	QDomNode particlesGroupRootNode = iRIC::getChildNode(node, "SolverParticlesGroupV4");
	if (! particlesGroupRootNode.isNull() && m_particleGroupRootDataItem != nullptr) {
		m_particleGroupRootDataItem->loadFromProjectMainFile(particlesGroupRootNode);
	}
	QDomNode srNode = iRIC::getChildNode(node, "StringResult");
	if (!srNode.isNull()) {
		m_stringDataItem->loadFromProjectMainFile(srNode);
	}
}

void Post3dWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_zoneName.c_str());
	writer.writeStartElement("Shape");
	m_shapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (m_contourGroupTopItem != nullptr) {
		writer.writeStartElement("ScalarNodeV4");
		m_contourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_cellContourGroupTopItem != nullptr) {
		writer.writeStartElement("ScalarCellV4");
		m_cellContourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_scalarGroupDataItem != nullptr) {
		writer.writeStartElement("Isosurfaces");
		m_scalarGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_arrowTopDataItem != nullptr) {
		writer.writeStartElement("ArrowGroupV4");
		m_arrowTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_streamlineGroupDataItem != nullptr) {
		writer.writeStartElement("StreamlineGroup");
		m_streamlineGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_particleGroupDataItem != nullptr) {
		writer.writeStartElement("ParticleGroup");
		m_particleGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_particlesDataItem != nullptr) {
		writer.writeStartElement("SolverParticlesV4");
		m_particlesDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_particleGroupRootDataItem != nullptr) {
		writer.writeStartElement("SolverParticlesGroupV4");
		m_particleGroupRootDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeStartElement("StringResult");
	m_stringDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post3dWindowZoneDataItem::informSelection(VTKGraphicsView* v)
{
	m_shapeDataItem->informSelection(v);
}

void Post3dWindowZoneDataItem::informDeselection(VTKGraphicsView* v)
{
	m_shapeDataItem->informDeselection(v);
}

v4PostZoneDataContainer* Post3dWindowZoneDataItem::v4DataContainer()
{
	return postSolutionInfo()->v4ZoneContainer3D(m_zoneName);
}

void Post3dWindowZoneDataItem::update()
{
	m_shapeDataItem->update();

	if (m_contourGroupTopItem != nullptr) {
		m_contourGroupTopItem->update();
	}
	if (m_cellContourGroupTopItem != nullptr) {
		m_cellContourGroupTopItem->update();
	}
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->update();
	}
	if (m_arrowTopDataItem != nullptr) {
		m_arrowTopDataItem->update();
	}
	if (m_streamlineGroupDataItem != nullptr) {
		m_streamlineGroupDataItem->update();
	}
	if (m_particleGroupDataItem != nullptr) {
		m_particleGroupDataItem->update();
	}
	if (m_particlesDataItem != nullptr) {
		m_particlesDataItem->update();
	}
	if (m_particleGroupRootDataItem != nullptr) {
		m_particleGroupRootDataItem->update();
	}
	m_stringDataItem->update();
}
