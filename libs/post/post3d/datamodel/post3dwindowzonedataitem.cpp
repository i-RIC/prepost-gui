#include "../post3dwindowdatamodel.h"
#include "../post3dwindowgraphicsview.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowgridshapedataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowifacecontourgrouptopdataitem.h"
#include "post3dwindowjfacecontourgrouptopdataitem.h"
#include "post3dwindowkfacecontourgrouptopdataitem.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindownodescalargrouptopdataitem.h"
#include "post3dwindownodevectorarrowtopdataitem.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post3dwindowparticlegrouprootdataitem.h"
#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowstringresultdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindoezonedataitem_impl.h"

#include <guibase/vtkgridedgeutil.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include <QAction>
#include <QDomNode>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QIcon>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

Post3dWindowZoneDataItem::Impl::Impl(const std::string& zoneName) :
	m_shapeDataItem {nullptr},
	m_contourGroupTopItem {nullptr},
	m_cellContourGroupTopItem {nullptr},
	m_iFaceContourGroupTopItem {nullptr},
	m_jFaceContourGroupTopItem {nullptr},
	m_kFaceContourGroupTopItem {nullptr},
	m_scalarGroupDataItem {nullptr},
	m_arrowTopDataItem {nullptr},
	m_streamlineGroupDataItem {nullptr},
	m_particleGroupDataItem {nullptr},
	m_particlesDataItem {nullptr},
	m_particleGroupRootDataItem {nullptr},
	m_stringDataItem {nullptr},
	m_edgeMapper {vtkPolyDataMapper::New()},
	m_edgeActor {vtkActor::New()},
	m_zoneName {zoneName}
{
	vtkGridEdgeUtil::setupActor(m_edgeActor);
	m_edgeActor->SetMapper(m_edgeMapper);
	m_edgeActor->VisibilityOff();
}

Post3dWindowZoneDataItem::Impl::~Impl()
{
	m_edgeMapper->Delete();
	m_edgeActor->Delete();
}

Post3dWindowZoneDataItem::Post3dWindowZoneDataItem(const std::string& zoneName, Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {zoneName.c_str(), QIcon(":/images/iconGrid.png"), parent},
	impl {new Impl {zoneName}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	renderer()->AddActor(impl->m_edgeActor);

	impl->m_shapeDataItem = new Post3dWindowGridShapeDataItem(this);

	auto cont = v4DataContainer();
	auto grid = cont->gridData();
	if (grid->scalarValueExists(v4SolutionGrid::Position::Node)) {
		impl->m_contourGroupTopItem = new Post3dWindowContourGroupTopDataItem(this);
		impl->m_scalarGroupDataItem = new Post3dWindowNodeScalarGroupTopDataItem(this);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::CellCenter)) {
		impl->m_cellContourGroupTopItem = new Post3dWindowCellContourGroupTopDataItem(this);
	}

	if (grid->scalarValueExists(v4SolutionGrid::Position::IFace)) {
		impl->m_iFaceContourGroupTopItem = new Post3dWindowIFaceContourGroupTopDataItem(this);
	}
	if (grid->scalarValueExists(v4SolutionGrid::Position::JFace)) {
		impl->m_jFaceContourGroupTopItem = new Post3dWindowJFaceContourGroupTopDataItem(this);
	}
	if (grid->scalarValueExists(v4SolutionGrid::Position::KFace)) {
		impl->m_kFaceContourGroupTopItem = new Post3dWindowKFaceContourGroupTopDataItem(this);
	}

	if (grid->vectorValueExists(v4SolutionGrid::Position::Node)) {
		impl->m_arrowTopDataItem = new Post3dWindowNodeVectorArrowTopDataItem(this);
		impl->m_streamlineGroupDataItem = new Post3dWindowNodeVectorStreamlineGroupStructuredDataItem(this);
		impl->m_particleGroupDataItem = new Post3dWindowNodeVectorParticleGroupStructuredDataItem(this);
	}

	if (cont->particleData() != nullptr) {
		impl->m_particlesDataItem = new Post3dWindowParticlesTopDataItem(this);
		impl->m_particlesDataItem->setup();
	}
	if (cont->particleGroupMap().size() > 0) {
		impl->m_particleGroupRootDataItem = new Post3dWindowParticleGroupRootDataItem(this);
	}

	impl->m_stringDataItem = new Post3dWindowStringResultDataItem(this);

	addChildItem(impl->m_shapeDataItem);
	addChildItem(impl->m_contourGroupTopItem);
	addChildItem(impl->m_scalarGroupDataItem);
	addChildItem(impl->m_cellContourGroupTopItem);
	addChildItem(impl->m_iFaceContourGroupTopItem);
	addChildItem(impl->m_jFaceContourGroupTopItem);
	addChildItem(impl->m_kFaceContourGroupTopItem);
	addChildItem(impl->m_arrowTopDataItem);
	addChildItem(impl->m_streamlineGroupDataItem);
	addChildItem(impl->m_particleGroupDataItem);
	addChildItem(impl->m_particlesDataItem);
	addChildItem(impl->m_particleGroupRootDataItem);
	addChildItem(impl->m_stringDataItem);
}

Post3dWindowZoneDataItem::~Post3dWindowZoneDataItem()
{
	delete impl->m_stringDataItem;

	renderer()->RemoveActor(impl->m_edgeActor);
}

const std::string& Post3dWindowZoneDataItem::zoneName() const
{
	return impl->m_zoneName;
}

Post3dWindowGridTypeDataItem* Post3dWindowZoneDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post3dWindowGridTypeDataItem*> (parent());
}

Post3dWindowGridShapeDataItem* Post3dWindowZoneDataItem::gridShapeDataItem() const
{
	return impl->m_shapeDataItem;
}

Post3dWindowContourGroupTopDataItem* Post3dWindowZoneDataItem::contourGroupTopItem() const
{
	return impl->m_contourGroupTopItem;
}

Post3dWindowCellContourGroupTopDataItem* Post3dWindowZoneDataItem::cellContourGroupTopItem() const
{
	return impl->m_cellContourGroupTopItem;
}

Post3dWindowIFaceContourGroupTopDataItem* Post3dWindowZoneDataItem::iFaceContourGroupTopItem() const
{
	return impl->m_iFaceContourGroupTopItem;
}

Post3dWindowJFaceContourGroupTopDataItem* Post3dWindowZoneDataItem::jFaceContourGroupTopItem() const
{
	return impl->m_jFaceContourGroupTopItem;
}

Post3dWindowKFaceContourGroupTopDataItem* Post3dWindowZoneDataItem::kFaceContourGroupTopItem() const
{
	return impl->m_kFaceContourGroupTopItem;
}

Post3dWindowNodeScalarGroupTopDataItem* Post3dWindowZoneDataItem::scalarGroupDataItem() const
{
	return impl->m_scalarGroupDataItem;
}

Post3dWindowNodeVectorArrowTopDataItem* Post3dWindowZoneDataItem::arrowTopDataItem() const
{
	return impl->m_arrowTopDataItem;
}

Post3dWindowNodeVectorStreamlineGroupDataItem* Post3dWindowZoneDataItem::streamlineGroupDataItem() const
{
	return impl->m_streamlineGroupDataItem;
}

Post3dWindowNodeVectorParticleGroupDataItem* Post3dWindowZoneDataItem::particleGroupDataItem() const
{
	return impl->m_particleGroupDataItem;
}

Post3dWindowParticlesTopDataItem* Post3dWindowZoneDataItem::particlesDataItem() const
{
	return impl->m_particlesDataItem;
}

Post3dWindowStringResultDataItem* Post3dWindowZoneDataItem::stringDataItem() const
{
	return impl->m_stringDataItem;
}

void Post3dWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		impl->m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	QDomNode contoursNode = iRIC::getChildNode(node, "ScalarNodeV4");
	if (!contoursNode.isNull() && impl->m_contourGroupTopItem != nullptr) {
		impl->m_contourGroupTopItem->loadFromProjectMainFile(contoursNode);
	}
	QDomNode cellContoursNode = iRIC::getChildNode(node, "ScalarCellV4");
	if (!cellContoursNode.isNull() && impl->m_cellContourGroupTopItem != nullptr) {
		impl->m_cellContourGroupTopItem->loadFromProjectMainFile(cellContoursNode);
	}
	QDomNode iFaceContoursNode = iRIC::getChildNode(node, "ScalarIFaceV4");
	if (!iFaceContoursNode.isNull() && impl->m_iFaceContourGroupTopItem != nullptr) {
		impl->m_iFaceContourGroupTopItem->loadFromProjectMainFile(iFaceContoursNode);
	}
	QDomNode jFaceContoursNode = iRIC::getChildNode(node, "ScalarJFaceV4");
	if (!jFaceContoursNode.isNull() && impl->m_jFaceContourGroupTopItem != nullptr) {
		impl->m_jFaceContourGroupTopItem->loadFromProjectMainFile(jFaceContoursNode);
	}
	QDomNode kFaceContoursNode = iRIC::getChildNode(node, "ScalarKFaceV4");
	if (!kFaceContoursNode.isNull() && impl->m_kFaceContourGroupTopItem != nullptr) {
		impl->m_kFaceContourGroupTopItem->loadFromProjectMainFile(kFaceContoursNode);
	}
	QDomNode isosurfacesNode = iRIC::getChildNode(node, "Isosurfaces");
	if (! isosurfacesNode.isNull() && impl->m_scalarGroupDataItem != nullptr) {
		// multi-isosurfaces
		impl->m_scalarGroupDataItem->loadFromProjectMainFile(isosurfacesNode);
	} else {
		// single-isosurface
		QDomNode scalarGroupNode = iRIC::getChildNode(node, "ScalarGroup");
		if (! scalarGroupNode.isNull() && impl->m_scalarGroupDataItem != nullptr) {
			impl->m_scalarGroupDataItem->loadFromProjectMainFile(scalarGroupNode);
		}
	}
	QDomNode arrowGroupTopNode = iRIC::getChildNode(node, "ArrowGroupV4");
	if (! arrowGroupTopNode.isNull() && impl->m_arrowTopDataItem != nullptr) {
		impl->m_arrowTopDataItem->loadFromProjectMainFile(arrowGroupTopNode);
	}
	QDomNode streamlineGroupNode = iRIC::getChildNode(node, "StreamlineGroup");
	if (! streamlineGroupNode.isNull() && impl->m_streamlineGroupDataItem != nullptr) {
		impl->m_streamlineGroupDataItem->loadFromProjectMainFile(streamlineGroupNode);
	}
	QDomNode particleGroupNode = iRIC::getChildNode(node, "ParticleGroup");
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
	QDomNode srNode = iRIC::getChildNode(node, "StringResult");
	if (!srNode.isNull()) {
		impl->m_stringDataItem->loadFromProjectMainFile(srNode);
	}
}

void Post3dWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", impl->m_zoneName.c_str());
	writer.writeStartElement("Shape");
	impl->m_shapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (impl->m_contourGroupTopItem != nullptr) {
		writer.writeStartElement("ScalarNodeV4");
		impl->m_contourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_cellContourGroupTopItem != nullptr) {
		writer.writeStartElement("ScalarCellV4");
		impl->m_cellContourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_iFaceContourGroupTopItem != nullptr) {
		writer.writeStartElement("ScalarIFaceV4");
		impl->m_iFaceContourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_jFaceContourGroupTopItem != nullptr) {
		writer.writeStartElement("ScalarJFaceV4");
		impl->m_jFaceContourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_kFaceContourGroupTopItem != nullptr) {
		writer.writeStartElement("ScalarKFaceV4");
		impl->m_kFaceContourGroupTopItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_scalarGroupDataItem != nullptr) {
		writer.writeStartElement("Isosurfaces");
		impl->m_scalarGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_arrowTopDataItem != nullptr) {
		writer.writeStartElement("ArrowGroupV4");
		impl->m_arrowTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_streamlineGroupDataItem != nullptr) {
		writer.writeStartElement("StreamlineGroup");
		impl->m_streamlineGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_particleGroupDataItem != nullptr) {
		writer.writeStartElement("ParticleGroup");
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
	writer.writeStartElement("StringResult");
	impl->m_stringDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post3dWindowZoneDataItem::informSelection(VTKGraphicsView* v)
{
	impl->m_shapeDataItem->informSelection(v);
}

void Post3dWindowZoneDataItem::informDeselection(VTKGraphicsView* v)
{
	impl->m_shapeDataItem->informDeselection(v);
}

v4PostZoneDataContainer* Post3dWindowZoneDataItem::v4DataContainer()
{
	return postSolutionInfo()->v4ZoneContainer3D(impl->m_zoneName);
}

void Post3dWindowZoneDataItem::setEdgeFocus(vtkIdType i, vtkIdType j, vtkIdType k)
{
	auto cont = v4DataContainer();
	if (cont == nullptr) {return;}
	auto gd = cont->gridData();
	if (gd == nullptr) {return;}
	auto g = gd->grid();
	if (g == nullptr) {return;}
	auto g2 = dynamic_cast<v4Structured3dGrid*> (g);
	auto polyData = g2->extractEdgeData(i, j, k);
	impl->m_edgeMapper->SetInputData(polyData);
	polyData->Delete();
	impl->m_edgeActor->VisibilityOn();
}

void Post3dWindowZoneDataItem::clearEdgeFocus()
{
	impl->m_edgeActor->VisibilityOff();
}

void Post3dWindowZoneDataItem::update()
{
	impl->m_shapeDataItem->update();

	if (impl->m_contourGroupTopItem != nullptr) {
		impl->m_contourGroupTopItem->update();
	}
	if (impl->m_cellContourGroupTopItem != nullptr) {
		impl->m_cellContourGroupTopItem->update();
	}
	if (impl->m_scalarGroupDataItem != nullptr) {
		impl->m_scalarGroupDataItem->update();
	}
	if (impl->m_arrowTopDataItem != nullptr) {
		impl->m_arrowTopDataItem->update();
	}
	if (impl->m_streamlineGroupDataItem != nullptr) {
		impl->m_streamlineGroupDataItem->update();
	}
	if (impl->m_particleGroupDataItem != nullptr) {
		impl->m_particleGroupDataItem->update();
	}
	if (impl->m_particlesDataItem != nullptr) {
		impl->m_particlesDataItem->update();
	}
	if (impl->m_particleGroupRootDataItem != nullptr) {
		impl->m_particleGroupRootDataItem->update();
	}
	impl->m_stringDataItem->update();
}
