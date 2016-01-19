#include "../post3dwindowdatamodel.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowarrowgroupdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowgridshapedataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
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
	m_contourGroupItem {nullptr},
	m_scalarGroupDataItem {nullptr},
	m_arrowGroupDataItem {nullptr},
	m_streamlineGroupDataItem {nullptr},
	m_particleGroupDataItem {nullptr},
	m_particlesDataItem {nullptr},
	m_zoneName (zoneName),
	m_zoneNumber {zoneNumber}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_shapeDataItem = new Post3dWindowGridShapeDataItem(this);

	PostZoneDataContainer* cont = dataContainer();
	if (cont->scalarValueExists()) {
		m_contourGroupItem = new Post3dWindowContourGroupDataItem(this);
		m_scalarGroupDataItem = new Post3dWindowNodeScalarGroupDataItem(this);
	}

	if (cont->vectorValueExists()) {
		m_arrowGroupDataItem = new Post3dWindowArrowGroupDataItem(this);
		m_streamlineGroupDataItem = new Post3dWindowNodeVectorStreamlineGroupStructuredDataItem(this);
		m_particleGroupDataItem = new Post3dWindowNodeVectorParticleGroupStructuredDataItem(this);
	}

	if (cont->particleData() != nullptr) {
		m_particlesDataItem = new Post3dWindowParticlesTopDataItem(this);
	}

	m_childItems.append(m_shapeDataItem);

	if (cont->scalarValueExists()) {
		m_childItems.append(m_contourGroupItem);
		m_childItems.append(m_scalarGroupDataItem);
	}

	if (cont->vectorValueExists()) {
		m_childItems.append(m_arrowGroupDataItem);
		m_childItems.append(m_streamlineGroupDataItem);
		m_childItems.append(m_particleGroupDataItem);
	}

	if (m_particlesDataItem != nullptr) {
		m_childItems.append(m_particlesDataItem);
	}
}

void Post3dWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	QDomNode contourGroupNode = iRIC::getChildNode(node, "ContourGroup");
	if (! contourGroupNode.isNull() && m_contourGroupItem != nullptr) {
		m_contourGroupItem->loadFromProjectMainFile(contourGroupNode);
	}
	QDomNode scalarGroupNode = iRIC::getChildNode(node, "ScalarGroup");
	if (! scalarGroupNode.isNull() && m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->loadFromProjectMainFile(scalarGroupNode);
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

	if (m_contourGroupItem != nullptr) {
		writer.writeStartElement("ContourGroup");
		m_contourGroupItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_scalarGroupDataItem != nullptr) {
		writer.writeStartElement("ScalarGroup");
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

	if (m_contourGroupItem != nullptr) {
		m_contourGroupItem->update();
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
}
