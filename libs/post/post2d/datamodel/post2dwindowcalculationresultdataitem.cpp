#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowgraphgroupdataitem.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowiedgescalargrouptopdataitem.h"
#include "post2dwindowjedgescalargrouptopdataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
#include "post2dwindownodevectorarrowgrouptopdataitem.h"
#include "post2dwindownodevectorparticlegroupstructureddataitem.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"
#include "post2dwindowparticlegrouprootdataitem.h"
#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowpolydatatopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowcalculationresultdataitem_impl.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/xmlsupport.h>
#include <postbase/string/poststringresultdataitem.h>

Post2dWindowCalculationResultDataItem::Post2dWindowCalculationResultDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowGridDataItemI {tr("Calculation Result"), parent},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	auto grid = cont->gridData();

	impl->m_gridShapeDataItem = new Post2dWindowGridShapeDataItem(this);

	if (grid->scalarValueExists(v4SolutionGrid::Position::Node)) {
		impl->m_nodeScalarGroupTopDataItem = new Post2dWindowNodeScalarGroupTopDataItem(this);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::CellCenter)) {
		impl->m_cellScalarGroupTopDataItem = new Post2dWindowCellScalarGroupTopDataItem(this);
		impl->m_cellScalarGroupTopDataItem->setupChildren();
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::IFace)) {
		impl->m_iEdgeScalarGroupTopDataItem = new Post2dWindowIEdgeScalarGroupTopDataItem(this);
		impl->m_iEdgeScalarGroupTopDataItem->setupChildren();
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::JFace)) {
		impl->m_jEdgeScalarGroupTopDataItem = new Post2dWindowJEdgeScalarGroupTopDataItem(this);
		impl->m_jEdgeScalarGroupTopDataItem->setupChildren();
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::Node) && dynamic_cast<v4Structured2dGrid*> (grid->grid()) != nullptr) {
		impl->m_graphGroupDataItem = new Post2dWindowGraphGroupDataItem(this);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::Node)) {
		impl->m_arrowGroupDataItem = new Post2dWindowNodeVectorArrowGroupTopDataItem(this);
		auto data = grid->grid();
		if (dynamic_cast<v4Structured2dGrid*> (data) != nullptr) {
			impl->m_streamlineGroupDataItem = new Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(this);
			impl->m_particleGroupDataItem = new Post2dWindowNodeVectorParticleGroupStructuredDataItem(this);
		} else {
			impl->m_streamlineGroupDataItem = new Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(this);
			impl->m_particleGroupDataItem = new Post2dWindowNodeVectorParticleGroupUnstructuredDataItem(this);
		}
	}
	if (cont->particleData() != nullptr) {
		impl->m_particlesDataItem = new Post2dWindowParticlesTopDataItem(this);
		impl->m_particlesDataItem->setup();
	}

	if (cont->particleGroupMap().size() > 0) {
		impl->m_particleGroupRootDataItem = new Post2dWindowParticleGroupRootDataItem(this);
	}

	if (cont->polyDataMap().size() > 0) {
		impl->m_polyDataDataItem = new Post2dWindowPolyDataTopDataItem(this);
	}

	impl->m_stringDataItem = new PostStringResultDataItem(this);

	addChildItem(impl->m_gridShapeDataItem);
	addChildItem(impl->m_arrowGroupDataItem);
	addChildItem(impl->m_particleGroupDataItem);
	addChildItem(impl->m_particlesDataItem);
	addChildItem(impl->m_particleGroupRootDataItem);
	addChildItem(impl->m_polyDataDataItem);
	addChildItem(impl->m_streamlineGroupDataItem);
	addChildItem(impl->m_graphGroupDataItem);
	addChildItem(impl->m_nodeScalarGroupTopDataItem);
	addChildItem(impl->m_cellScalarGroupTopDataItem);
	addChildItem(impl->m_iEdgeScalarGroupTopDataItem);
	addChildItem(impl->m_jEdgeScalarGroupTopDataItem);
	addChildItem(impl->m_stringDataItem);
}

Post2dWindowCalculationResultDataItem::~Post2dWindowCalculationResultDataItem()
{}

void Post2dWindowCalculationResultDataItem::informSelection(VTKGraphicsView* v)
{
	impl->m_gridShapeDataItem->informSelection(v);
}

void Post2dWindowCalculationResultDataItem::informDeselection(VTKGraphicsView* v)
{
	impl->m_gridShapeDataItem->informDeselection(v);
}

void Post2dWindowCalculationResultDataItem::update(bool noParticle)
{
	impl->m_gridShapeDataItem->update();
	if (impl->m_nodeScalarGroupTopDataItem != nullptr) {
		impl->m_nodeScalarGroupTopDataItem->update();
	}
	if (impl->m_cellScalarGroupTopDataItem != nullptr) {
		impl->m_cellScalarGroupTopDataItem->update();
	}
	if (impl->m_cellScalarGroupTopDataItem != nullptr) {
		impl->m_cellScalarGroupTopDataItem->update();
	}
	if (impl->m_iEdgeScalarGroupTopDataItem != nullptr) {
		impl->m_iEdgeScalarGroupTopDataItem->update();
	}
	if (impl->m_jEdgeScalarGroupTopDataItem != nullptr) {
		impl->m_jEdgeScalarGroupTopDataItem->update();
	}
	if (impl->m_graphGroupDataItem != nullptr) {
		impl->m_graphGroupDataItem->update();
	}
	if (impl->m_arrowGroupDataItem != nullptr) {
		impl->m_arrowGroupDataItem->update();
	}
	if (impl->m_streamlineGroupDataItem != nullptr) {
		impl->m_streamlineGroupDataItem->update();
	}
	if (impl->m_particleGroupDataItem != nullptr && ! noParticle) {
		impl->m_particleGroupDataItem->update();
	}
	if (impl->m_particlesDataItem != nullptr) {
		impl->m_particlesDataItem->update();
	}
	if (impl->m_particleGroupRootDataItem != nullptr) {
		impl->m_particleGroupRootDataItem->update();
	}
	if (impl->m_polyDataDataItem != nullptr) {
		impl->m_polyDataDataItem->update();
	}

	impl->m_stringDataItem->update();
}

v4Grid2d* Post2dWindowCalculationResultDataItem::grid() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	return dynamic_cast<v4Grid2d*> (cont->gridData()->grid());
}

Post2dWindowZoneDataItem* Post2dWindowCalculationResultDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

Post2dWindowGridShapeDataItem* Post2dWindowCalculationResultDataItem::gridShapeDataItem() const
{
	return impl->m_gridShapeDataItem;
}

Post2dWindowNodeScalarGroupTopDataItem* Post2dWindowCalculationResultDataItem::nodeScalarGroupTopDataItem() const
{
	return impl->m_nodeScalarGroupTopDataItem;
}

Post2dWindowCellScalarGroupTopDataItem* Post2dWindowCalculationResultDataItem::cellScalarGroupTopDataItem() const
{
	return impl->m_cellScalarGroupTopDataItem;
}

Post2dWindowIEdgeScalarGroupTopDataItem* Post2dWindowCalculationResultDataItem::iEdgeScalarGroupTopDataItem() const
{
	return impl->m_iEdgeScalarGroupTopDataItem;
}

Post2dWindowJEdgeScalarGroupTopDataItem* Post2dWindowCalculationResultDataItem::jEdgeScalarGroupTopDataItem() const
{
	return impl->m_jEdgeScalarGroupTopDataItem;
}

Post2dWindowGraphGroupDataItem* Post2dWindowCalculationResultDataItem::graphGroupDataItem() const
{
	return impl->m_graphGroupDataItem;
}

Post2dWindowNodeVectorArrowGroupTopDataItem* Post2dWindowCalculationResultDataItem::arrowGroupDataItem() const
{
	return impl->m_arrowGroupDataItem;
}

Post2dWindowNodeVectorStreamlineGroupDataItem* Post2dWindowCalculationResultDataItem::streamlineDataItem() const
{
	return impl->m_streamlineGroupDataItem;
}

Post2dWindowNodeVectorParticleGroupDataItem* Post2dWindowCalculationResultDataItem::particleGroupDataItem() const
{
	return impl->m_particleGroupDataItem;
}

Post2dWindowParticlesTopDataItem* Post2dWindowCalculationResultDataItem::particlesDataItem() const
{
	return impl->m_particlesDataItem;
}

Post2dWindowParticleGroupRootDataItem* Post2dWindowCalculationResultDataItem::particleGroupRootDataItem() const
{
	return impl->m_particleGroupRootDataItem;
}

Post2dWindowPolyDataTopDataItem* Post2dWindowCalculationResultDataItem::polyDataDataItem() const
{
	return impl->m_polyDataDataItem;
}

PostStringResultDataItem* Post2dWindowCalculationResultDataItem::stringDataItem() const
{
	return impl->m_stringDataItem;
}

void Post2dWindowCalculationResultDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		impl->m_gridShapeDataItem->loadFromProjectMainFile(shapeNode);
	}

	QDomNode scalarNodeNode = iRIC::getChildNode(node, "ScalarNodeV4");
	if (! scalarNodeNode.isNull() && impl->m_nodeScalarGroupTopDataItem != nullptr) {
		impl->m_nodeScalarGroupTopDataItem->loadFromProjectMainFile(scalarNodeNode);
	}

	QDomNode scalarCellNode = iRIC::getChildNode(node, "ScalarCellV4");
	if (! scalarCellNode.isNull() && impl->m_nodeScalarGroupTopDataItem != nullptr) {
		impl->m_nodeScalarGroupTopDataItem->loadFromProjectMainFile(scalarCellNode);
	}

	QDomNode scalarIEdgeNode = iRIC::getChildNode(node, "ScalarEdgeIV4");
	if (! scalarIEdgeNode.isNull() && impl->m_iEdgeScalarGroupTopDataItem != nullptr) {
		impl->m_iEdgeScalarGroupTopDataItem->loadFromProjectMainFile(scalarIEdgeNode);
	}

	QDomNode scalarJEdgeNode = iRIC::getChildNode(node, "ScalarEdgeJV4");
	if (! scalarJEdgeNode.isNull() && impl->m_jEdgeScalarGroupTopDataItem != nullptr) {
		impl->m_jEdgeScalarGroupTopDataItem->loadFromProjectMainFile(scalarJEdgeNode);
	}

	QDomNode graphNode = iRIC::getChildNode(node, "GraphGroup");
	if (! graphNode.isNull() && impl->m_graphGroupDataItem != nullptr) {
		impl->m_graphGroupDataItem->loadFromProjectMainFile(graphNode);
	}

	QDomNode arrowGroupNode = iRIC::getChildNode(node, "ArrowGroupV4");
	if (! arrowGroupNode.isNull() && impl->m_arrowGroupDataItem != nullptr) {
		impl->m_arrowGroupDataItem->loadFromProjectMainFile(arrowGroupNode);
	}

	QDomNode arrowGroupTopNode = iRIC::getChildNode(node, "ArrowGroupTopV4");
	if (! arrowGroupTopNode.isNull() && impl->m_arrowGroupDataItem != nullptr) {
		impl->m_arrowGroupDataItem->loadFromProjectMainFile(arrowGroupTopNode);
	}

	QDomNode streamlineGroupNode = iRIC::getChildNode(node, "StreamlineGroup");
	if (! streamlineGroupNode.isNull() && impl->m_streamlineGroupDataItem != nullptr) {
		impl->m_streamlineGroupDataItem->loadFromProjectMainFile(streamlineGroupNode);
	}

	QDomNode particleGroupNode = iRIC::getChildNode(node, "ParticleGroupV4");
	if (! particleGroupNode.isNull() && impl->m_particleGroupDataItem != nullptr) {
		impl->m_particleGroupDataItem->loadFromProjectMainFile(particleGroupNode);
	}

	QDomNode particlesNode = iRIC::getChildNode(node, "SolverParticlesV4");
	if (! particlesNode.isNull() && impl->m_particlesDataItem != nullptr) {
		impl->m_particlesDataItem->loadFromProjectMainFile(particlesNode);
	}

	QDomNode particlesGroupRootNode = iRIC::getChildNode(node, "SolverParticlesGroupV4");
	if (! particlesGroupRootNode.isNull() && impl->m_particleGroupRootDataItem != nullptr) {
		impl->m_particleGroupRootDataItem->loadFromProjectMainFile(particlesGroupRootNode);
	}

	QDomNode polyDataNode = iRIC::getChildNode(node, "SolverPolyDataV4");
	if (! polyDataNode.isNull() && impl->m_polyDataDataItem != nullptr) {
		impl->m_polyDataDataItem->loadFromProjectMainFile(polyDataNode);
	}

	QDomNode srNode = iRIC::getChildNode(node, "StringResult");
	if (!srNode.isNull()) {
		impl->m_stringDataItem->loadFromProjectMainFile(srNode);
	}
}

void Post2dWindowCalculationResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("Shape");
	impl->m_gridShapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (impl->m_nodeScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarNodeV4");
		impl->m_nodeScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_cellScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarCellV4");
		impl->m_cellScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_iEdgeScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarEdgeIV4");
		impl->m_iEdgeScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_jEdgeScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarEdgeJV4");
		impl->m_jEdgeScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_graphGroupDataItem != nullptr) {
		writer.writeStartElement("GraphGroup");
		impl->m_graphGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_arrowGroupDataItem != nullptr) {
		writer.writeStartElement("ArrowGroupTopV4");
		impl->m_arrowGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_streamlineGroupDataItem != nullptr) {
		writer.writeStartElement("StreamlineGroup");
		impl->m_streamlineGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_particleGroupDataItem != nullptr) {
		writer.writeStartElement("ParticleGroupV4");
		impl->m_particleGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_particlesDataItem != nullptr) {
		writer.writeStartElement("SolverParticlesV4");
		impl->m_particlesDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_particleGroupRootDataItem != nullptr) {
		writer.writeStartElement("SolverParticlesGroupV4");
		impl->m_particleGroupRootDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	if (impl->m_polyDataDataItem != nullptr) {
		writer.writeStartElement("SolverPolyDataV4");
		impl->m_polyDataDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	writer.writeStartElement("StringResult");
	impl->m_stringDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}
