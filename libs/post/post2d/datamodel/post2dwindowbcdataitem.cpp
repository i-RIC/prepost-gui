#include "post2dwindowbcdataitem.h"
#include "post2dwindowdataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "private/post2dwindowbcdataitem_impl.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/postcontainer/v4postzonedatabc.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/errormessage.h>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextMapper.h>

Post2dWindowBCDataItem::Impl::Impl(Post2dWindowBCDataItem* item) :
	m_data {vtkPolyData::New()},
	m_mapper {vtkPolyDataMapper::New()},
	m_actor {vtkActor::New()},
	m_nameActor {vtkActor2D::New()},
	m_nameMapper {vtkTextMapper::New()}
{
	m_mapper->SetScalarVisibility(false);
	auto prop = m_actor->GetProperty();
	prop->SetLighting(false);

	m_mapper->SetInputData(m_data);
	m_actor->SetMapper(m_mapper);
}

Post2dWindowBCDataItem::Impl::~Impl()
{
	m_data->Delete();
	m_mapper->Delete();
	m_actor->Delete();
	m_nameActor->Delete();
	m_nameMapper->Delete();
}

Post2dWindowBCDataItem::Post2dWindowBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, v4PostZoneDataBC* inputGridBC, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {inputGridBC->caption().c_str(), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	impl->m_condition = cond;
	impl->m_inputGridBC = inputGridBC;

	setupActors();
	updateElements();
}

Post2dWindowBCDataItem::~Post2dWindowBCDataItem()
{
	delete impl;
}

void Post2dWindowBCDataItem::setupActors()
{
	// node, edge, cell related settings
	QColor color = impl->m_inputGridBC->defaultColor();
	auto prop = impl->m_actor->GetProperty();
	prop->SetColor(color.redF(), color.greenF(), color.blueF());
	prop->SetOpacity(impl->m_inputGridBC->defaultOpacity());

	renderer()->AddActor(impl->m_actor);
	actorCollection()->AddItem(impl->m_actor);

	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		prop->SetPointSize(impl->m_inputGridBC->defaultPointSize());
		prop->SetRepresentationToPoints();
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		// nothing to do
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		prop->SetLineWidth(impl->m_inputGridBC->defaultPointSize());
		prop->SetRepresentationToWireframe();
	}

	// name related settings
	vtkTextProperty* tprop = impl->m_nameMapper->GetTextProperty();

	tprop->SetColor(color.redF(), color.greenF(), color.blueF());

	tprop->SetJustificationToCentered();
	tprop->SetVerticalJustificationToCentered();

	impl->m_nameActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
	impl->m_nameActor->SetMapper(impl->m_nameMapper);

	renderer()->AddActor2D(impl->m_nameActor);
	actor2DCollection()->AddItem(impl->m_nameActor);
}

void Post2dWindowBCDataItem::doUpdateActorSetting()
{

}

void Post2dWindowBCDataItem::updateElements()
{
	Post2dWindowInputGridDataItem* tmpparent = dynamic_cast<Post2dWindowInputGridDataItem*>(parent()->parent());

	auto d = impl->m_data;
	auto grid = tmpparent->grid()->vtkData()->data();
	d->Reset();
	d->SetPoints(grid->GetPoints());

	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		auto ca = vtkSmartPointer<vtkCellArray>::New();
		for (auto index : impl->m_inputGridBC->indices()) {
			ca->InsertNextCell(1, &index);
		}
		d->SetVerts(ca);
	}
	else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		for (auto index : impl->m_inputGridBC->indices()) {
			vtkCell* cell = grid->GetCell(index);
			ca->InsertNextCell(cell->GetPointIds());
		}
		d->SetPolys(ca);
	}
	else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		vtkIdType nodes[2];
		for (const auto& e : impl->m_inputGridBC->edges()) {
			nodes[0] = e.vertex1();
			nodes[1] = e.vertex2();
			ca->InsertNextCell(2, &(nodes[0]));
		}
		d->SetLines(ca);
	}
	d->Modified();
}

void Post2dWindowBCDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{

}

void Post2dWindowBCDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{

}