#include "../post2dwindow.h"
#include "../post2dwindowdatamodel.h"
#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcellflaggroupdataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowgraphgroupdataitem.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindownodevectorparticlegroupstructureddataitem.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"
#include "post2dwindowparticlegrouprootdataitem.h"
#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowpolydatatopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegercell.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroptioncell.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
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
#include <QTime>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

#include <cgnslib.h>
#include <iriclib.h>

Post2dWindowZoneDataItem::Post2dWindowZoneDataItem(const std::string& zoneName, int zoneNumber, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {zoneName.c_str(), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	m_shapeDataItem {nullptr},
	m_scalarGroupTopDataItem {nullptr},
	m_cellScalarGroupTopDataItem {nullptr},
	m_arrowGroupDataItem {nullptr},
	m_streamlineGroupDataItem {nullptr},
	m_particleGroupDataItem {nullptr},
	m_cellFlagGroupDataItem {nullptr},
	m_particlesDataItem {nullptr},
	m_particleGroupRootDataItem {nullptr},
	m_polyDataDataItem {nullptr},
	m_graphGroupDataItem {nullptr},
	m_currentParticlesData {nullptr},
	m_zoneName (zoneName),
	m_zoneNumber {zoneNumber},
	m_attributeBrowserFixed {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_shapeDataItem = new Post2dWindowGridShapeDataItem(this);

	PostZoneDataContainer* cont = dataContainer();

	if (cont->scalarValueExists()) {
		m_scalarGroupTopDataItem = new Post2dWindowNodeScalarGroupTopDataItem(this);
	}

	if (cont->cellScalarValueExists()) {
		m_cellScalarGroupTopDataItem = new Post2dWindowCellScalarGroupTopDataItem(this);
	}

	if (cont->scalarValueExists() && dynamic_cast<vtkStructuredGrid*> (cont->data()) != nullptr) {
		m_graphGroupDataItem = new Post2dWindowGraphGroupDataItem(this);
	}

	if (cont->vectorValueExists()) {
		vtkPointSet* data = cont->data();
		if (dynamic_cast<vtkUnstructuredGrid*> (data) != nullptr){
			m_arrowGroupDataItem = new Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(this);
			m_streamlineGroupDataItem = new Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(this);
			m_particleGroupDataItem = new Post2dWindowNodeVectorParticleGroupUnstructuredDataItem(this);
		} else {
			m_arrowGroupDataItem = new Post2dWindowNodeVectorArrowGroupStructuredDataItem(this);
			m_streamlineGroupDataItem = new Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(this);
			m_particleGroupDataItem = new Post2dWindowNodeVectorParticleGroupStructuredDataItem(this);
		}
	}
	if (cont->particleData() != nullptr) {
		m_particlesDataItem = new Post2dWindowParticlesTopDataItem(this);
		m_particlesDataItem->setup();
	}
	if (cont->particleGroupMap().size() > 0) {
		m_particleGroupRootDataItem = new Post2dWindowParticleGroupRootDataItem(this);
	}
	if (cont->polyDataMap().size() > 0) {
		m_polyDataDataItem = new Post2dWindowPolyDataTopDataItem(this);
	}

	m_cellFlagGroupDataItem = new Post2dWindowCellFlagGroupDataItem(this);

	addChildItem(m_shapeDataItem);
	addChildItem(m_arrowGroupDataItem);
	addChildItem(m_particleGroupDataItem);
	addChildItem(m_streamlineGroupDataItem);
	addChildItem(m_particlesDataItem);
	addChildItem(m_particleGroupRootDataItem);
	addChildItem(m_polyDataDataItem);
	addChildItem(m_graphGroupDataItem);
	addChildItem(m_scalarGroupTopDataItem);
	addChildItem(m_cellFlagGroupDataItem);
	addChildItem(m_cellScalarGroupTopDataItem);

	m_showAttributeBrowserActionForNodeResult = new QAction(Post2dWindowZoneDataItem::tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserActionForNodeResult, SIGNAL(triggered()), this, SLOT(showNodeAttributeBrowser()));

	m_showAttributeBrowserActionForCellResult = new QAction(Post2dWindowZoneDataItem::tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserActionForCellResult, SIGNAL(triggered()), this, SLOT(showCellAttributeBrowser()));

	m_showAttributeBrowserActionForCellInput = new QAction(Post2dWindowZoneDataItem::tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserActionForCellInput, SIGNAL(triggered()), this, SLOT(showCellAttributeBrowser()));

	m_showAttributeBrowserActionForPolyDataResult = new QAction(tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserActionForPolyDataResult, SIGNAL(triggered()), this, SLOT(showPolyDataBrowser()));

	setupActors();
	updateRegionPolyData();
}

Post2dWindowZoneDataItem::~Post2dWindowZoneDataItem()
{
	renderer()->RemoveActor(m_regionActor);
	delete m_polyDataDataItem;
}

void Post2dWindowZoneDataItem::setupActors()
{
	vtkProperty* prop;

	m_regionPolyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> tmppoints = vtkSmartPointer<vtkPoints>::New();
	m_regionPolyData->SetPoints(tmppoints);

	m_regionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_regionMapper->SetInputData(m_regionPolyData);

	m_regionActor = vtkSmartPointer<vtkActor>::New();
	m_regionActor->SetMapper(m_regionMapper);
	prop = m_regionActor->GetProperty();
	prop->SetOpacity(0);
	prop->SetColor(0, 0, 0);
	m_regionActor->VisibilityOff();
	renderer()->AddActor(m_regionActor);
}

void Post2dWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {
		m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	// old contour
	QDomNode scalarGroupNode = iRIC::getChildNode(node, "ScalarGroup");
	if (! scalarGroupNode.isNull() && m_scalarGroupTopDataItem != nullptr) {
		m_scalarGroupTopDataItem->loadFromProjectMainFile(scalarGroupNode);
	}
	// new contour
	QDomNode contourGroupNode = iRIC::getChildNode(node, "Contours");
	if (! contourGroupNode.isNull() && m_scalarGroupTopDataItem != nullptr) {
		m_scalarGroupTopDataItem->loadFromProjectMainFile(contourGroupNode);
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
	QDomNode cellFlagGroupNode = iRIC::getChildNode(node, "CellFlagGroup");
	if (! cellFlagGroupNode.isNull() && m_cellFlagGroupDataItem != nullptr) {
		m_cellFlagGroupDataItem->loadFromProjectMainFile(cellFlagGroupNode);
	}
	QDomNode particlesNode = iRIC::getChildNode(node, "SolverParticles");
	if (! particlesNode.isNull() && m_particlesDataItem != nullptr) {
		m_particlesDataItem->loadFromProjectMainFile(particlesNode);
	}
	QDomNode polyDataNode = iRIC::getChildNode(node, "SolverPolyData");
	if (! polyDataNode.isNull() && m_polyDataDataItem != nullptr) {
		m_polyDataDataItem->loadFromProjectMainFile(polyDataNode);
	}
	QDomNode graphNode = iRIC::getChildNode(node, "GraphGroup");
	if (! graphNode.isNull() && m_graphGroupDataItem != nullptr) {
		m_graphGroupDataItem->loadFromProjectMainFile(graphNode);
	}
	QDomNode cellScalarNode = iRIC::getChildNode(node, "ScalarCellCenter");
	if (!cellScalarNode.isNull() && m_cellScalarGroupTopDataItem != nullptr) {
		m_cellScalarGroupTopDataItem->loadFromProjectMainFile(cellScalarNode);
	}
}

void Post2dWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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
	if (m_cellFlagGroupDataItem != nullptr) {
		writer.writeStartElement("CellFlagGroup");
		m_cellFlagGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_particlesDataItem != nullptr) {
		writer.writeStartElement("SolverParticles");
		m_particlesDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_polyDataDataItem != nullptr) {
		writer.writeStartElement("SolverPolyData");
		m_polyDataDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_graphGroupDataItem != nullptr) {
		writer.writeStartElement("GraphGroup");
		m_graphGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_cellScalarGroupTopDataItem != nullptr) {
		writer.writeStartElement("ScalarCellCenter");
		m_cellScalarGroupTopDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowZoneDataItem::addCustomMenuItems(QMenu* /*menu*/)
{

}

void Post2dWindowZoneDataItem::informSelection(VTKGraphicsView* v)
{
	m_shapeDataItem->informSelection(v);
}

void Post2dWindowZoneDataItem::informDeselection(VTKGraphicsView* v)
{
	m_shapeDataItem->informDeselection(v);
}

PostZoneDataContainer* Post2dWindowZoneDataItem::dataContainer()
{
	return postSolutionInfo()->zoneContainer2D(m_zoneName);
}

vtkPolyData* Post2dWindowZoneDataItem::filteredData() const
{
	return m_filteredData;
}

bool Post2dWindowZoneDataItem::isMasked() const
{
	return m_isMasked;
}

int Post2dWindowZoneDataItem::zoneNumber() const
{
	return m_zoneNumber;
}

std::string Post2dWindowZoneDataItem::zoneName() const
{
	return m_zoneName;
}

void Post2dWindowZoneDataItem::update(bool noparticle)
{
	double xmin, xmax, ymin, ymax;
	dataModel()->graphicsView()->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);

	PostZoneDataContainer* cont = dataContainer();
	if (cont != nullptr && cont->data() != nullptr) {
		m_filteredData = dataContainer()->filteredData(xmin, xmax, ymin, ymax, m_isMasked);
		m_filteredData->UnRegister(0);
	}

	QTime time;
	time.start();
	m_shapeDataItem->update();
	qDebug("Grid shape: %d", time.elapsed());

	if (m_scalarGroupTopDataItem != nullptr) {
		time.restart();
		m_scalarGroupTopDataItem->update();
		qDebug("Contour shape: %d", time.elapsed());
	}
	if (m_cellScalarGroupTopDataItem != nullptr) {
		time.restart();
		m_cellScalarGroupTopDataItem->update();
		qDebug("Cell Contour shape: %d", time.elapsed());
	}
	if (m_arrowGroupDataItem != nullptr) {
		time.restart();
		m_arrowGroupDataItem->update();
		qDebug("Arrows: %d", time.elapsed());
	}
	if (m_streamlineGroupDataItem != nullptr) {
		time.restart();
		m_streamlineGroupDataItem->update();
		qDebug("Streamlines: %d", time.elapsed());
	}
	if (m_particleGroupDataItem != nullptr && ! noparticle) {
		time.restart();
		m_particleGroupDataItem->update();
		qDebug("Particles: %d", time.elapsed());
	}
	if (m_cellFlagGroupDataItem != nullptr) {
		time.restart();
		m_cellFlagGroupDataItem->update();
		qDebug("Cell flags: %d", time.elapsed());
	}
	if (m_particlesDataItem != nullptr) {
		time.restart();
		m_particlesDataItem->update();
		qDebug("Solver Particles: %d", time.elapsed());
	}
	if (m_polyDataDataItem != nullptr) {
		time.restart();
		m_polyDataDataItem->update();
		qDebug("Solver PolyData: %d", time.elapsed());
	}
	if (m_graphGroupDataItem != nullptr) {
		time.restart();
		m_graphGroupDataItem->update();
		qDebug("Graphs: %d", time.elapsed());
	}
	updateRegionPolyData();
}

Post2dWindowGridShapeDataItem* Post2dWindowZoneDataItem::gridShapeDataItem() const
{
	return m_shapeDataItem;
}

Post2dWindowNodeScalarGroupTopDataItem* Post2dWindowZoneDataItem::scalarGroupTopDataItem() const
{
	return m_scalarGroupTopDataItem;
}

Post2dWindowNodeVectorArrowGroupDataItem* Post2dWindowZoneDataItem::arrowGroupDataItem() const
{
	return m_arrowGroupDataItem;
}

Post2dWindowNodeVectorStreamlineGroupDataItem* Post2dWindowZoneDataItem::streamlineDataItem() const
{
	return m_streamlineGroupDataItem;
}

Post2dWindowNodeVectorParticleGroupDataItem* Post2dWindowZoneDataItem::particleDataItem() const
{
	return m_particleGroupDataItem;
}

Post2dWindowCellFlagGroupDataItem* Post2dWindowZoneDataItem::cellFlagGroupDataItem() const
{
	return m_cellFlagGroupDataItem;
}

Post2dWindowCellScalarGroupTopDataItem* Post2dWindowZoneDataItem::cellScalarGroupTopDataItem() const
{
	return m_cellScalarGroupTopDataItem;
}

Post2dWindowParticlesTopDataItem* Post2dWindowZoneDataItem::particlesDataItem() const
{
	return m_particlesDataItem;
}

Post2dWindowPolyDataTopDataItem* Post2dWindowZoneDataItem::polyDataDataItem() const
{
	return m_polyDataDataItem;
}

Post2dWindowGraphGroupDataItem* Post2dWindowZoneDataItem::graphGroupDataItem() const
{
	return m_graphGroupDataItem;
}

void Post2dWindowZoneDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size() + 1);
}

void Post2dWindowZoneDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_regionActor->SetPosition(0, 0, range.min());

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

	if (cont->particleData() != nullptr) {
		// Particles (auto)
		max = min - divWidth * gapRate;
		min = max - divWidth;
		if (m_particlesDataItem != nullptr) {
			r = m_particlesDataItem->zDepthRange();
			r.setRange(min, max);
			m_particlesDataItem->setZDepthRange(r);
		}
	}
	if (cont->polyDataMap().size() > 0) {
		// PolyData
		max = min - divWidth * gapRate;
		min = max - divWidth;
		if (m_polyDataDataItem != nullptr) {
			r = m_polyDataDataItem->zDepthRange();
			r.setRange(min, max);
			m_polyDataDataItem->setZDepthRange(r);
		}
	}

	if (cont->vectorValueExists()) {
		// Particles
		max = min - divWidth * gapRate;
		min = max - divWidth;
		if (m_particleGroupDataItem != nullptr) {
			r = m_particleGroupDataItem->zDepthRange();
			r.setRange(min, max);
			m_particleGroupDataItem->setZDepthRange(r);
		}

		// Arrows
		max = min - divWidth * gapRate;
		min = max - divWidth;
		r = m_arrowGroupDataItem->zDepthRange();
		r.setRange(min, max);
		m_arrowGroupDataItem->setZDepthRange(r);

		// Streamlines
		max = min - divWidth * gapRate;
		min = max - divWidth;
		r = m_streamlineGroupDataItem->zDepthRange();
		r.setRange(min, max);
		m_streamlineGroupDataItem->setZDepthRange(r);
	}

	if (m_graphGroupDataItem != nullptr) {
		// graphs
		max = min - divWidth * gapRate;
		min = max - divWidth;
		r = m_graphGroupDataItem->zDepthRange();
		r.setRange(min, max);
		m_graphGroupDataItem->setZDepthRange(r);
	}

	// cells
	max = min - divWidth * gapRate;
	min = max - divWidth;
	r = m_cellFlagGroupDataItem->zDepthRange();
	r.setRange(min, max);
	m_cellFlagGroupDataItem->setZDepthRange(r);

	// Contour
	if (cont->scalarValueExists()) {
		max = min - divWidth * gapRate;
		min = max - divWidth;
		r = m_scalarGroupTopDataItem->zDepthRange();
		r.setRange(min, max);
		m_scalarGroupTopDataItem->setZDepthRange(r);
	}

	// Cell Contour
	if (cont->cellScalarValueExists()) {
		max = min - divWidth * gapRate;
		min = max - divWidth;
		r = m_cellScalarGroupTopDataItem->zDepthRange();
		r.setRange(min, max);
		m_cellScalarGroupTopDataItem->setZDepthRange(r);
	}
}

void Post2dWindowZoneDataItem::initNodeResultAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	PostZoneDataContainer* cont = dataContainer();
	if (cont == nullptr) {return;}

	vtkStructuredGrid* sgrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	vtkUnstructuredGrid* usgrid = dynamic_cast<vtkUnstructuredGrid*>(cont->data());
	if (sgrid != nullptr) {
		pb->view()->resetForVertex(true);
	} else if (usgrid != nullptr) {
		pb->view()->resetForVertex(false);
	}
}

void Post2dWindowZoneDataItem::clearNodeResultAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->hideAll();
	m_attributeBrowserFixed = false;
}

void Post2dWindowZoneDataItem::fixNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType vid = findVertex(p, v);
	m_attributeBrowserFixed = (vid >= 0);
	if (vid < 0) {
		// no point is near.
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	dataContainer()->data()->GetPoint(vid, vertex);

	updateNodeResultAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void Post2dWindowZoneDataItem::updateNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType vid = findVertex(p, v);
	if (vid < 0) {
		// no point is near.
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	dataContainer()->data()->GetPoint(vid, vertex);

	updateNodeResultAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void Post2dWindowZoneDataItem::initCellResultAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	PostZoneDataContainer* cont = dataContainer();
	if (cont == nullptr) {return;}

	vtkStructuredGrid* sgrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	vtkUnstructuredGrid* usgrid = dynamic_cast<vtkUnstructuredGrid*>(cont->data());
	if (sgrid != nullptr) {
		pb->view()->resetForVertex(true);
	} else if (usgrid != nullptr) {
		pb->view()->resetForVertex(false);
	}
}

void Post2dWindowZoneDataItem::clearCellResultAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->hideAll();
	m_attributeBrowserFixed = false;
}

void Post2dWindowZoneDataItem::fixCellResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType cellid = findCell(p, v);
	m_attributeBrowserFixed = (cellid >= 0);
	if (cellid < 0) {
		// no point is near.
		pb->view()->resetAttributes();
		return;
	}
	updateCellResultAttributeBrowser(cellid, v);
}

void Post2dWindowZoneDataItem::updateCellResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType cellid = findCell(p, v);
	if (cellid < 0) {
		// no point is near.
		pb->view()->resetAttributes();
		return;
	}
	updateCellResultAttributeBrowser(cellid, v);
}

vtkIdType Post2dWindowZoneDataItem::findVertex(const QPoint& p, VTKGraphicsView* v)
{
	double x = p.x();
	double y = p.y();
	Post2dWindowGraphicsView* v2 = dynamic_cast<Post2dWindowGraphicsView*>(v);
	v2->viewportToWorld(x, y);

	PostZoneDataContainer* cont = dataContainer();
	vtkIdType vid = cont->data()->FindPoint(x, y, 0);
	if (vid < 0) {
		// no point is near.
		return -1;
	}
	double vertex[3];
	cont->data()->GetPoint(vid, vertex);

	QVector2D vec1(x, y);
	QVector2D vec2(vertex[0], vertex[1]);
	double distance = (vec2 - vec1).length();
	double limitDist = v2->stdRadius(iRIC::nearRadius());
	if (distance > limitDist) {
		// no point is near.
		return -1;
	}
	return vid;
}

void Post2dWindowZoneDataItem::updateNodeResultAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* /*v*/)
{
	PostZoneDataContainer* cont = dataContainer();
	QList<PropertyBrowserAttribute> atts;

	int count = cont->data()->GetPointData()->GetNumberOfArrays();
	for (int i = 0; i < count; ++i) {
		vtkAbstractArray* arr = cont->data()->GetPointData()->GetAbstractArray(i);
		vtkDataArray* da = dynamic_cast<vtkDataArray*>(arr);
		if (da == nullptr) {continue;}
		if (da->GetNumberOfComponents() == 1) {
			// scalar value
			double val = da->GetComponent(vid, 0);
			PropertyBrowserAttribute att(da->GetName(), val);
			atts.append(att);
		} else if (da->GetNumberOfComponents() == 3) {
			// vector value
			double val = da->GetComponent(vid, 0);
			QString attName = da->GetName();
			attName.append("X");
			PropertyBrowserAttribute att(attName, val);
			atts.append(att);

			val = da->GetComponent(vid, 1);
			attName = da->GetName();
			attName.append("Y");
			att = PropertyBrowserAttribute(attName, val);
			atts.append(att);
		}
	}
	vtkStructuredGrid* sgrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	vtkUnstructuredGrid* usgrid = dynamic_cast<vtkUnstructuredGrid*>(cont->data());

	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (sgrid != nullptr) {
		int i, j, k;
		cont->getNodeIJKIndex(vid, &i, &j, &k);
		pb->view()->setVertexAttributes(i, j, x, y, atts);
	} else if (usgrid != nullptr) {
		pb->view()->setVertexAttributes(vid, x, y, atts);
	}
}

void Post2dWindowZoneDataItem::updateCellResultAttributeBrowser(vtkIdType cellid, VTKGraphicsView*)
{
	QList<PropertyBrowserAttribute> atts;

	auto cont = dataContainer();
	auto cellData = cont->data()->GetCellData();

	int count = cellData->GetNumberOfArrays();
	for (int i = 0; i < count; ++i) {
		auto arr = cellData->GetAbstractArray(i);
		auto da = dynamic_cast<vtkDataArray*>(arr);
		if (da == nullptr) {continue;}
		if (da->GetNumberOfComponents() == 1) {
			// scalar value
			double val = da->GetComponent(cellid, 0);
			PropertyBrowserAttribute att(da->GetName(), val);
			atts.append(att);
		} else if (da->GetNumberOfComponents() == 3) {
			// vector value
			double val = da->GetComponent(cellid, 0);
			QString attName = da->GetName();
			attName.append("X");
			PropertyBrowserAttribute att(attName, val);
			atts.append(att);

			val = da->GetComponent(cellid, 1);
			attName = da->GetName();
			attName.append("Y");
			att = PropertyBrowserAttribute(attName, val);
			atts.append(att);
		}
	}

	QPolygonF polygon;
	vtkCell* cell = cont->data()->GetCell(cellid);
	vtkPoints* points = cont->data()->GetPoints();
	double* vv;
	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		vv = points->GetPoint(cell->GetPointIds()->GetId(i));
		polygon.append(QPointF(*vv, *(vv + 1)));
	}
	vv = points->GetPoint(cell->GetPointIds()->GetId(0));
	polygon.append(QPointF(*vv, *(vv + 1)));

	auto sgrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	auto usgrid = dynamic_cast<vtkUnstructuredGrid*>(cont->data());

	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (sgrid != nullptr) {
		int i, j, k;
		cont->getCellIJKIndex(cellid, &i, &j, &k);
		pb->view()->setCellAttributes(i, j, polygon, atts);
	} else if (usgrid != nullptr) {
		pb->view()->setCellAttributes(cellid, polygon, atts);
	}
}

void Post2dWindowZoneDataItem::initCellInputAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	PostZoneDataContainer* cont = dataContainer();
	vtkStructuredGrid* sgrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	vtkUnstructuredGrid* usgrid = dynamic_cast<vtkUnstructuredGrid*>(cont->data());
	if (sgrid != nullptr) {
		pb->view()->resetForCell(true);
	} else if (usgrid) {
		pb->view()->resetForCell(false);
	}
}

void Post2dWindowZoneDataItem::clearCellInputAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->hideAll();
}

void Post2dWindowZoneDataItem::fixCellInputAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType cellid = findCell(p, v);
	m_attributeBrowserFixed = (cellid >= 0);
	if (cellid < 0) {
		// it is not inside a cell.
		pb->view()->resetAttributes();
		return;
	}
	updateCellInputAttributeBrowser(cellid, v);
}

void Post2dWindowZoneDataItem::updateCellInputAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType cellid = findCell(p, v);
	if (cellid < 0) {
		// it is not inside a cell.
		pb->view()->resetAttributes();
		return;
	}
	updateCellInputAttributeBrowser(cellid, v);
}

vtkIdType Post2dWindowZoneDataItem::findCell(const QPoint& p, VTKGraphicsView* v)
{
	double x = p.x();
	double y = p.y();
	Post2dWindowGraphicsView* v2 = dynamic_cast<Post2dWindowGraphicsView*>(v);
	v2->viewportToWorld(x, y);

	PostZoneDataContainer* cont = dataContainer();
	double point[3];
	point[0] = x; point[1] = y; point[2] = 0;
	vtkCell* hintCell = nullptr;
	double pcoords[4];
	double weights[4];
	int subid;
	return cont->data()->FindCell(point, hintCell, 0, 1e-4, subid, pcoords, weights);
}

void Post2dWindowZoneDataItem::updateCellInputAttributeBrowser(vtkIdType cellid, VTKGraphicsView* /*v*/)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();

	PostZoneDataContainer* cont = dataContainer();
	QList<PropertyBrowserAttribute> atts;

	SolverDefinitionGridType* gt = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent())->gridType();
	const QList<SolverDefinitionGridAttribute*>& conds = gt->gridAttributes();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		const SolverDefinitionGridAttribute* cond = *it;
		if (cond->position() != SolverDefinitionGridAttribute::CellCenter) {continue;}
		const SolverDefinitionGridAttributeInteger* icond = dynamic_cast<const SolverDefinitionGridAttributeInteger*>(cond);
		if (icond == nullptr) {continue;}

		const IntegerEnumLoader* el = dynamic_cast<const IntegerEnumLoader*>(cond);

		vtkIntArray* cellVals = vtkIntArray::SafeDownCast(cont->data()->GetCellData()->GetArray(icond->name().c_str()));
		int val = cellVals->GetValue(cellid);
		if (el != nullptr) {
			PropertyBrowserAttribute att(cond->caption(), el->enumerations().value(val));
			atts.append(att);
		} else {
			PropertyBrowserAttribute att(cond->caption(), val);
			atts.append(att);
		}
	}
	QPolygonF polygon;
	vtkCell* cell = cont->data()->GetCell(cellid);
	vtkPoints* points = cont->data()->GetPoints();
	double* vv;
	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		vv = points->GetPoint(cell->GetPointIds()->GetId(i));
		polygon.append(QPointF(*vv, *(vv + 1)));
	}
	vv = points->GetPoint(cell->GetPointIds()->GetId(0));
	polygon.append(QPointF(*vv, *(vv + 1)));

	vtkStructuredGrid* sgrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
	vtkUnstructuredGrid* usgrid = dynamic_cast<vtkUnstructuredGrid*>(cont->data());

	if (sgrid != nullptr) {
		int i, j, k;
		cont->getCellIJKIndex(cellid, &i, &j, &k);
		pb->view()->setCellAttributes(i, j, polygon, atts);
	} else if (usgrid != nullptr) {
		pb->view()->setCellAttributes(cellid, polygon, atts);
	}
}

void Post2dWindowZoneDataItem::initParticleResultAttributeBrowser(vtkPolyData* data)
{
	m_currentParticlesData = data;
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->resetForParticle();
}

void Post2dWindowZoneDataItem::clearParticleResultAttributeBrowser()
{
	m_currentParticlesData = nullptr;
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->hideAll();
}

void Post2dWindowZoneDataItem::fixParticleResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	if (m_currentParticlesData == nullptr) {return;}

	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType pid = findParticle(p, v);
	m_attributeBrowserFixed = (pid >= 0);
	if (pid < 0) {
		// no particle is near.
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	m_currentParticlesData->GetPoint(pid, vertex);

	updateParticleResultAttributeBrowser(pid, vertex[0], vertex[1], v);
}

void Post2dWindowZoneDataItem::updateParticleResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType pid = findParticle(p, v);
	if (pid < 0) {
		// no particle is near.
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	dataContainer()->particleData()->GetPoint(pid, vertex);

	updateParticleResultAttributeBrowser(pid, vertex[0], vertex[1], v);
}

void Post2dWindowZoneDataItem::initPolyDataResultAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->resetForPolyData();
}

void Post2dWindowZoneDataItem::clearPolyDataResultAttributeBrowser()
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->hideAll();
}

void Post2dWindowZoneDataItem::fixPolyDataResultAttributeBrowser(const std::string& name, const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType pid = findPolyDataCell(name, p, v);
	m_attributeBrowserFixed = (pid >= 0);
	if (pid < 0) {
		// no cell is near.
		pb->view()->resetAttributes();
		return;
	}

	updatePolyDataResultAttributeBrowser(name, p, v);
}

void Post2dWindowZoneDataItem::updatePolyDataResultAttributeBrowser(const std::string& name, const QPoint& p, VTKGraphicsView* v)
{
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType pid = findPolyDataCell(name, p, v);
	if (pid < 0) {
		// no cell is near
		pb->view()->resetAttributes();
		return;
	}
	updatePolyDataResultAttributeBrowser(name, pid, v);
}

vtkIdType Post2dWindowZoneDataItem::findParticle(const QPoint& p, VTKGraphicsView* v)
{
	if (m_currentParticlesData == nullptr) {return -1;}
	double x = p.x();
	double y = p.y();
	Post2dWindowGraphicsView* v2 = dynamic_cast<Post2dWindowGraphicsView*>(v);
	v2->viewportToWorld(x, y);

	vtkIdType pid = m_currentParticlesData->FindPoint(x, y, 0);
	if (pid < 0) {
		// no particle is near.
		return -1;
	}
	double vertex[3];
	m_currentParticlesData->GetPoint(pid, vertex);

	QVector2D vec1(x, y);
	QVector2D vec2(vertex[0], vertex[1]);
	double distance = (vec2 - vec1).length();
	double limitDist = v2->stdRadius(iRIC::nearRadius());
	if (distance > limitDist) {
		// no point is near.
		return -1;
	}
	return pid;
}

int Post2dWindowZoneDataItem::findPolyDataCell(const std::string& name, const QPoint& p, VTKGraphicsView* v)
{
	double x = p.x();
	double y = p.y();
	Post2dWindowGraphicsView* v2 = dynamic_cast<Post2dWindowGraphicsView*>(v);
	v2->viewportToWorld(x, y);

	PostZoneDataContainer* cont = dataContainer();
	double point[3];
	point[0] = x; point[1] = y; point[2] = 0;
	vtkCell* hintCell = nullptr;
	double pcoords[4];
	double weights[4];
	double limitDist = v2->stdRadius(iRIC::nearRadius());
	double d2 = limitDist * limitDist;
	int subid;
	vtkIdType cellid = cont->polyData(name)->FindCell(point, hintCell, 0, d2, subid, pcoords, weights);
	if (cellid < 0) {return cellid;}

	return cont->polyDataCellIds(name).at(cellid);
}

void Post2dWindowZoneDataItem::updateParticleResultAttributeBrowser(vtkIdType pid, double x, double y, VTKGraphicsView* /*v*/)
{
	if (m_currentParticlesData == nullptr) {return;}

	QList<PropertyBrowserAttribute> atts;

	int count = m_currentParticlesData->GetPointData()->GetNumberOfArrays();
	for (int i = 0; i < count; ++i) {
		vtkAbstractArray* arr = m_currentParticlesData->GetPointData()->GetAbstractArray(i);
		vtkDataArray* da = dynamic_cast<vtkDataArray*>(arr);
		if (da == nullptr) {continue;}
		if (da->GetNumberOfComponents() == 1) {
			// scalar value
			double val = da->GetComponent(pid, 0);
			PropertyBrowserAttribute att(da->GetName(), val);
			atts.append(att);
		} else if (da->GetNumberOfComponents() == 3) {
			// vector value
			double val = da->GetComponent(pid, 0);
			QString attName = da->GetName();
			attName.append("X");
			PropertyBrowserAttribute att(attName, val);
			atts.append(att);

			val = da->GetComponent(pid, 1);
			attName = da->GetName();
			attName.append("Y");
			att = PropertyBrowserAttribute(attName, val);
			atts.append(att);
		}
	}
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();
	pb->view()->setParticleAttributes(pid, x, y, atts);
}

void Post2dWindowZoneDataItem::updatePolyDataResultAttributeBrowser(const std::string& name, int cellid, VTKGraphicsView* v)
{
	PostZoneDataContainer* cont = dataContainer();
	QList<PropertyBrowserAttribute> atts;
	auto polyData = cont->polyData(name);
	const auto& polyDataCellIds = cont->polyDataCellIds(name);
	auto s_it = std::lower_bound(polyDataCellIds.begin(), polyDataCellIds.end(), cellid);
	auto first_id = s_it - polyDataCellIds.begin();

	int count = polyData->GetCellData()->GetNumberOfArrays();

	for (int i = 0; i < count; ++i) {
		vtkAbstractArray* arr = polyData->GetCellData()->GetAbstractArray(i);
		vtkDataArray* da = dynamic_cast<vtkDataArray*>(arr);
		if (da == nullptr) {continue;}
		if (da->GetNumberOfComponents() == 1) {
			// scalar value
			double val = da->GetComponent(first_id, 0);
			PropertyBrowserAttribute att(da->GetName(), val);
			atts.append(att);
		} else if (da->GetNumberOfComponents() == 3) {
			// vector value
			double val = da->GetComponent(first_id, 0);
			QString attName = da->GetName();
			attName.append("X");
			PropertyBrowserAttribute att(attName, val);
			atts.append(att);

			val = da->GetComponent(first_id, 1);
			attName = da->GetName();
			attName.append("Y");
			att = PropertyBrowserAttribute(attName, val);
			atts.append(att);
		}
	}
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	PropertyBrowser* pb = w->propertyBrowser();

	std::vector<QPolygonF> polygons;
	for (auto it = s_it; *it == cellid; ++it) {
		auto tmp_id = it - polyDataCellIds.begin();
		vtkCell* cell = polyData->GetCell(tmp_id);
		auto line = dynamic_cast<vtkLine*>(cell);
		auto polyline = dynamic_cast<vtkPolyLine*>(cell);
		if (line != nullptr || polyline != nullptr) {
			auto points = cell->GetPoints();
			auto ids = cell->GetPointIds();
			std::vector<QPointF> l;
			for (vtkIdType i = 0; i < ids->GetNumberOfIds(); ++i) {
				double v[3];
				points->GetPoint(i, v);
				l.push_back(QPointF(v[0], v[1]));
			}
			pb->view()->setPolyDataAttributes(l, cellid, atts);
			return;
		} else {
			auto points = cell->GetPoints();
			auto ids = cell->GetPointIds();
			QPolygonF p;
			for (vtkIdType i = 0; i < ids->GetNumberOfIds(); ++i) {
				double v[3];
				points->GetPoint(i, v);
				p.push_back(QPointF(v[0], v[1]));
			}
			double v[3];
			points->GetPoint(0, v);
			p.push_back(QPointF(v[0], v[1]));

			polygons.push_back(p);
		}
	}
	pb->view()->setPolyDataAttributes(polygons, cellid, atts);
}

void Post2dWindowZoneDataItem::showNodeAttributeBrowser()
{
	initNodeResultAttributeBrowser();
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	w->propertyBrowser()->show();
}

void Post2dWindowZoneDataItem::showCellAttributeBrowser()
{
	initCellInputAttributeBrowser();
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	w->propertyBrowser()->show();
}

void Post2dWindowZoneDataItem::showPolyDataBrowser()
{
	initPolyDataResultAttributeBrowser();
	Post2dWindow* w = dynamic_cast<Post2dWindow*>(mainWindow());
	w->propertyBrowser()->show();
}

void Post2dWindowZoneDataItem::doViewOperationEndedGlobal(VTKGraphicsView* /*v*/)
{
	update(true);
}

void Post2dWindowZoneDataItem::updateRegionPolyData()
{
	PostZoneDataContainer* cont = dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}

	vtkPointSet* ds = dataContainer()->data();
	double bounds[6];
	ds->GetBounds(bounds);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(4);
	points->InsertNextPoint(bounds[0], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[3], 0);
	points->InsertNextPoint(bounds[0], bounds[3], 0);
	m_regionPolyData->SetPoints(points);

	vtkIdType pts[4] = {0, 1, 2, 3};
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(4, pts);
	m_regionPolyData->SetPolys(cells);
	m_regionPolyData->Modified();
	actorCollection()->RemoveItem(m_regionActor);
	actorCollection()->AddItem(m_regionActor);
	updateVisibilityWithoutRendering();
}

QAction* Post2dWindowZoneDataItem::showAttributeBrowserActionForCellInput() const
{
	return m_showAttributeBrowserActionForCellInput;
}

QAction* Post2dWindowZoneDataItem::showAttributeBrowserActionForNodeResult() const
{
	return m_showAttributeBrowserActionForNodeResult;
}

QAction* Post2dWindowZoneDataItem::showAttributeBrowserActionForCellResult() const
{
	return m_showAttributeBrowserActionForCellResult;
}

QAction* Post2dWindowZoneDataItem::showAttributeBrowserActionForPolyDataResult() const
{
	return m_showAttributeBrowserActionForPolyDataResult;
}
