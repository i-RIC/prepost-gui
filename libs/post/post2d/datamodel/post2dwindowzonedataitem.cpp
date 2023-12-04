#include "../post2dwindow.h"
#include "../post2dwindowdatamodel.h"
#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/grid/v4polydata2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeinteger.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroption.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

Post2dWindowZoneDataItem::Post2dWindowZoneDataItem(const std::string& zoneName, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {zoneName.c_str(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_inputGridDataItem {nullptr},
	m_resultDataItem {nullptr},
	m_zoneName (zoneName)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = v4DataContainer();

	if (cont->inputGridData() != nullptr) {
		m_inputGridDataItem = new Post2dWindowInputGridDataItem(this);
	}

	m_resultDataItem = new Post2dWindowCalculationResultDataItem(this);

	addChildItem(m_inputGridDataItem);
	addChildItem(m_resultDataItem);

	setupActors();
	updateRegionPolyData();
}

Post2dWindowZoneDataItem::~Post2dWindowZoneDataItem()
{
	renderer()->RemoveActor(m_regionActor);
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
	QDomNode inputNode = iRIC::getChildNode(node, "InputGrid");
	if (! inputNode.isNull() && m_inputGridDataItem != nullptr) {
		m_inputGridDataItem->loadFromProjectMainFile(inputNode);
	}

	QDomNode resultNode = iRIC::getChildNode(node, "CalculationResult");
	if (! resultNode.isNull()) {
		m_resultDataItem->loadFromProjectMainFile(resultNode);
	} else {
		m_resultDataItem->loadFromProjectMainFile(node);
	}
}

void Post2dWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_zoneName.c_str());

	if (m_inputGridDataItem != nullptr) {
		writer.writeStartElement("InputGrid");
		m_inputGridDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	writer.writeStartElement("CalculationResult");
	m_resultDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post2dWindowZoneDataItem::addCustomMenuItems(QMenu* /*menu*/)
{}

v4PostZoneDataContainer* Post2dWindowZoneDataItem::v4DataContainer()
{
	return postSolutionInfo()->v4ZoneContainer2D(m_zoneName);
}

bool Post2dWindowZoneDataItem::isMasked() const
{
	return m_isMasked;
}

std::string Post2dWindowZoneDataItem::zoneName() const
{
	return m_zoneName;
}

void Post2dWindowZoneDataItem::update(bool noParticle)
{
	double xmin, xmax, ymin, ymax;
	dataModel()->graphicsView()->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);

	auto v4Cont = v4DataContainer();
	if (v4Cont != nullptr) {
		auto ig = v4Cont->inputGridData();
		if (ig != nullptr) {
			auto grid2d_input = dynamic_cast<v4Grid2d*> (ig->grid());
			grid2d_input->updateFilteredData(xmin, xmax, ymin, ymax);
		}

		auto grid2d_result = dynamic_cast<v4Grid2d*> (v4Cont->gridData()->grid());
		grid2d_result->updateFilteredData(xmin, xmax, ymin, ymax);
	}

	if (m_inputGridDataItem != nullptr) {
		m_inputGridDataItem->update();
	}
	m_resultDataItem->update(noParticle);

	updateRegionPolyData();
}

Post2dWindowGridTypeDataItem* Post2dWindowZoneDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post2dWindowGridTypeDataItem*> (parent());
}

Post2dWindowInputGridDataItem* Post2dWindowZoneDataItem::inputGridDataItem() const
{
	return m_inputGridDataItem;
}

Post2dWindowCalculationResultDataItem* Post2dWindowZoneDataItem::resultDataItem() const
{
	return m_resultDataItem;
}

void Post2dWindowZoneDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size() + 1);
}

void Post2dWindowZoneDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_regionActor->SetPosition(0, 0, range.min());

	GraphicsWindowDataItem::assignActorZValues(range);
}

void Post2dWindowZoneDataItem::doViewOperationEndedGlobal(VTKGraphicsView* /*v*/)
{
	update(true);
}

void Post2dWindowZoneDataItem::updateRegionPolyData()
{
	auto cont = v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {return;}

	auto ds = cont->gridData()->grid()->vtkData()->data();
	double bounds[6];
	ds->GetBounds(bounds);

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(4);
	points->InsertNextPoint(bounds[0], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[3], 0);
	points->InsertNextPoint(bounds[0], bounds[3], 0);
	m_regionPolyData->SetPoints(points);

	vtkIdType pts[4] = {0, 1, 2, 3};
	auto cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(4, pts);
	m_regionPolyData->SetPolys(cells);
	m_regionPolyData->Modified();
	actorCollection()->RemoveItem(m_regionActor);
	actorCollection()->AddItem(m_regionActor);
	updateVisibilityWithoutRendering();
}
