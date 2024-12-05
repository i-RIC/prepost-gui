#include "../post2dwindow.h"
#include "../post2dwindowdatamodel.h"
#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowzonedataitem_impl.h"

#include <guicore/grid/v4polydata2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/postcontainer/postsolutioninfo.h>
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

Post2dWindowZoneDataItem::Impl::Impl(const std::string& zoneName) :
	m_regionPolyData {vtkPolyData::New()},
	m_regionMapper {vtkPolyDataMapper::New()},
	m_regionActor {vtkActor::New()},
	m_inputGridDataItem {nullptr},
	m_resultDataItem {nullptr},
	m_zoneName {zoneName}
{
	m_regionMapper->SetInputData(m_regionPolyData);
	m_regionActor->SetMapper(m_regionMapper);
}

Post2dWindowZoneDataItem::Impl::~Impl()
{
	m_regionPolyData->Delete();
	m_regionMapper->Delete();
	m_regionActor->Delete();
}

Post2dWindowZoneDataItem::Post2dWindowZoneDataItem(const std::string& zoneName, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {zoneName.c_str(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	impl {new Impl {zoneName}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = v4DataContainer();

	if (cont->inputGridData() != nullptr) {
		impl->m_inputGridDataItem = new Post2dWindowInputGridDataItem(this);
	}

	impl->m_resultDataItem = new Post2dWindowCalculationResultDataItem(this);

	addChildItem(impl->m_inputGridDataItem);
	addChildItem(impl->m_resultDataItem);

	setupActors();
	updateRegionPolyData();

	renderer()->AddActor(impl->m_regionActor);
}

Post2dWindowZoneDataItem::~Post2dWindowZoneDataItem()
{
	renderer()->RemoveActor(impl->m_regionActor);
}

void Post2dWindowZoneDataItem::setupActors()
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	impl->m_regionPolyData->SetPoints(points);

	auto prop = impl->m_regionActor->GetProperty();
	prop->SetOpacity(0);
	prop->SetColor(0, 0, 0);
	impl->m_regionActor->VisibilityOff();

	m_actorCollection->AddItem(impl->m_regionActor);
}

void Post2dWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode inputNode = iRIC::getChildNode(node, "InputGrid");
	if (! inputNode.isNull() && impl->m_inputGridDataItem != nullptr) {
		impl->m_inputGridDataItem->loadFromProjectMainFile(inputNode);
	}

	QDomNode resultNode = iRIC::getChildNode(node, "CalculationResult");
	if (! resultNode.isNull()) {
		impl->m_resultDataItem->loadFromProjectMainFile(resultNode);
	} else {
		impl->m_resultDataItem->loadFromProjectMainFile(node);
	}
}

void Post2dWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", impl->m_zoneName.c_str());

	if (impl->m_inputGridDataItem != nullptr) {
		writer.writeStartElement("InputGrid");
		impl->m_inputGridDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	writer.writeStartElement("CalculationResult");
	impl->m_resultDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Post2dWindowZoneDataItem::addCustomMenuItems(QMenu* /*menu*/)
{}

v4PostZoneDataContainer* Post2dWindowZoneDataItem::v4DataContainer()
{
	return postSolutionInfo()->v4ZoneContainer2D(impl->m_zoneName);
}

std::string Post2dWindowZoneDataItem::zoneName() const
{
	return impl->m_zoneName;
}

void Post2dWindowZoneDataItem::update(bool noParticle)
{
	double xmin, xmax, ymin, ymax;
	dataModel()->graphicsView()->getDrawnRegionWithMargin(&xmin, &xmax, &ymin, &ymax);

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

	if (impl->m_inputGridDataItem != nullptr) {
		impl->m_inputGridDataItem->update();
	}
	impl->m_resultDataItem->update(noParticle);

	updateRegionPolyData();
}

void Post2dWindowZoneDataItem::setEdgeFocus(vtkIdType i, vtkIdType j)
{
	impl->m_resultDataItem->setEdgeFocus(i, j);
}

void Post2dWindowZoneDataItem::clearEdgeFocus()
{
	impl->m_resultDataItem->clearEdgeFocus();
}

Post2dWindowGridTypeDataItem* Post2dWindowZoneDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post2dWindowGridTypeDataItem*> (parent());
}

Post2dWindowInputGridDataItem* Post2dWindowZoneDataItem::inputGridDataItem() const
{
	return impl->m_inputGridDataItem;
}

Post2dWindowCalculationResultDataItem* Post2dWindowZoneDataItem::resultDataItem() const
{
	return impl->m_resultDataItem;
}

void Post2dWindowZoneDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size() + 1);
}

void Post2dWindowZoneDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_regionActor->SetPosition(0, 0, range.min());

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
	impl->m_regionPolyData->SetPoints(points);

	vtkIdType pts[4] = {0, 1, 2, 3};
	auto cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(4, pts);
	impl->m_regionPolyData->SetPolys(cells);
	impl->m_regionPolyData->Modified();
	// updateVisibilityWithoutRendering();
}
