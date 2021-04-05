#include "../post3dwindowdatamodel.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowarrowgroupdataitem.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowgridshapedataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindownodescalargrouptopdataitem.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post3dwindowparticlegrouprootdataitem.h"
#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <postbase/string/poststringresultdataitem.h>

#include <QAction>
#include <QDomNode>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QIcon>
#include <QMenu>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

#include <cgnslib.h>
#include <iriclib.h>

Post3dWindowZoneDataItem::Post3dWindowZoneDataItem(const std::string& zoneName, int zoneNumber, Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {zoneName.c_str(), QIcon(":/images/iconGrid.png"), parent},
	m_shapeDataItem {nullptr},
	m_contourGroupTopItem {nullptr},
	m_cellContourGroupTopItem {nullptr},
	m_scalarGroupDataItem {nullptr},
	m_arrowGroupDataItem {nullptr},
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

	PostZoneDataContainer* cont = dataContainer();
	if (cont->scalarValueExists()) {
		m_contourGroupTopItem = new Post3dWindowContourGroupTopDataItem(this);
		m_scalarGroupDataItem = new Post3dWindowNodeScalarGroupTopDataItem(this);
	}

	if (cont->cellScalarValueExists()) {
		m_cellContourGroupTopItem = new Post3dWindowCellContourGroupTopDataItem(this);
	}

	if (cont->vectorValueExists()) {
		m_arrowGroupDataItem = new Post3dWindowArrowGroupDataItem(this);
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

	m_stringDataItem = new PostStringResultDataItem(this);

	m_childItems.push_back(m_shapeDataItem);

	if (cont->scalarValueExists()) {
		m_childItems.push_back(m_contourGroupTopItem);
		m_childItems.push_back(m_scalarGroupDataItem);
	}

	if (cont->cellScalarValueExists()) {
		m_childItems.push_back(m_cellContourGroupTopItem);
	}

	if (cont->vectorValueExists()) {
		m_childItems.push_back(m_arrowGroupDataItem);
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


int Post3dWindowZoneDataItem::zoneNumber() const {
	return m_zoneNumber;
}

const std::string& Post3dWindowZoneDataItem::zoneName() const
{
	return m_zoneName;
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

Post3dWindowArrowGroupDataItem* Post3dWindowZoneDataItem::arrowGroupDataItem() const
{
	return m_arrowGroupDataItem;
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

PostStringResultDataItem* Post3dWindowZoneDataItem::stringDataItem() const
{
	return m_stringDataItem;
}

void Post3dWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	QDomNode contoursNode = iRIC::getChildNode(node, "Contours");
	if (!contoursNode.isNull() && m_contourGroupTopItem != nullptr) {
		// multi-contours
		m_contourGroupTopItem->loadFromProjectMainFile(contoursNode);
	} else {
		// single-contour
		QDomNode contourGroupNode = iRIC::getChildNode(node, "ContourGroup");
		if (! contourGroupNode.isNull() && m_contourGroupTopItem != nullptr) {
			m_contourGroupTopItem->loadFromProjectMainFile(contourGroupNode);
		}
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
	QDomNode arrowGroupNode = iRIC::getChildNode(node, "ArrowGroup");
	if (! arrowGroupNode.isNull() && m_arrowGroupDataItem != nullptr) {
		m_arrowGroupDataItem->loadFromProjectMainFile(arrowGroupNode);
	}
	QDomNode streamlineGroupNode = iRIC::getChildNode(node, "StreamlineGroup");
	if (! streamlineGroupNode.isNull() && m_streamlineGroupDataItem != nullptr) {
		m_streamlineGroupDataItem->loadFromProjectMainFile(streamlineGroupNode);
	}
	QDomNode particleGroupNode = iRIC::getChildNode(node, "ParticleGroup");
	if (! particleGroupNode.isNull() && m_particleGroupDataItem != nullptr) {
		m_particleGroupDataItem->loadFromProjectMainFile(particleGroupNode);
	}
	QDomNode particlesNode = iRIC::getChildNode(node, "SolverParticles");
	if (! particlesNode.isNull() && m_particlesDataItem != nullptr) {
		m_particlesDataItem->loadFromProjectMainFile(particlesNode);
	}
}

void Post3dWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_zoneName.c_str());
	writer.writeStartElement("Shape");
	m_shapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (m_contourGroupTopItem != nullptr) {
		writer.writeStartElement("Contours");
		m_contourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_scalarGroupDataItem != nullptr) {
		writer.writeStartElement("Isosurfaces");
		m_scalarGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_arrowGroupDataItem != nullptr) {
		writer.writeStartElement("ArrowGroup");
		m_arrowGroupDataItem->saveToProjectMainFile(writer);
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
		writer.writeStartElement("SolverParticles");
		m_particlesDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowZoneDataItem::informSelection(VTKGraphicsView* v)
{
	m_shapeDataItem->informSelection(v);
}

void Post3dWindowZoneDataItem::informDeselection(VTKGraphicsView* v)
{
	m_shapeDataItem->informDeselection(v);
}

PostZoneDataContainer* Post3dWindowZoneDataItem::dataContainer()
{
	return postSolutionInfo()->zoneContainer3D(m_zoneName);
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
	if (m_arrowGroupDataItem != nullptr) {
		m_arrowGroupDataItem->update();
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
	m_stringDataItem->update();
}
