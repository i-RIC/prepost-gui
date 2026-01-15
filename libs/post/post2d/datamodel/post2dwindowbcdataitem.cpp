#include "post2dwindowbcdataitem.h"
#include "post2dwindowdataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "private/post2dwindowbcdataitem_impl.h"
#include "private/post2dwindowbcdataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/postcontainer/v4postzonedatabc.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/errormessage.h>
#include <misc/stringtool.h>

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

	impl->m_setting.opacity = inputGridBC->defaultOpacity();
	impl->m_setting.color = inputGridBC->defaultColor();
	impl->m_setting.showName = inputGridBC->defaultShowName();
	impl->m_setting.pointSize = inputGridBC->defaultPointSize();

	setupActors();
	updateElements();

	updateVisibilityWithoutRendering();
}

Post2dWindowBCDataItem::~Post2dWindowBCDataItem()
{
	delete impl;
}

v4PostZoneDataBC* Post2dWindowBCDataItem::inputGridBC() const
{
	return impl->m_inputGridBC;
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
	auto prop = impl->m_actor->GetProperty();

	prop->SetOpacity(impl->m_setting.opacity);
	prop->SetColor(impl->m_setting.color);
	prop->SetPointSize(impl->m_setting.pointSize);
	prop->SetLineWidth(impl->m_setting.pointSize);
	
	updateNameActorSettings();
}

void Post2dWindowBCDataItem::updateNameActorSettings()
{
	actor2DCollection()->RemoveItem(impl->m_nameActor);
	impl->m_nameActor->VisibilityOff();

	if (!impl->m_setting.showName) { return; }

	double centerv[3] = { 0, 0, 0 };
	Post2dWindowInputGridDataItem* tmpparent = dynamic_cast<Post2dWindowInputGridDataItem*>(parent()->parent());
	if (tmpparent->grid() == nullptr) { return; }
	vtkPointSet* pset = tmpparent->grid()->vtkData()->data();
	vtkPoints* points = pset->GetPoints();
	int pnum = 0;
	if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
		for (auto index : impl->m_inputGridBC->indices()) {
			double tmpv[3];
			points->GetPoint(index, tmpv);
			for (int i = 0; i < 3; ++i) {
				centerv[i] += tmpv[i];
			}
			++pnum;
		}
	}
	else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		for (auto index : impl->m_inputGridBC->indices()) {
			vtkCell* cell = pset->GetCell(index);
			for (int i = 0; i < cell->GetNumberOfPoints(); ++i) {
				double tmpv[3];
				points->GetPoint(cell->GetPointId(i), tmpv);
				for (int i = 0; i < 3; ++i) {
					centerv[i] += tmpv[i];
				}
				++pnum;
			}
		}
	}
	else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
		for (const auto& e : impl->m_inputGridBC->edges()) {
			double tmpv[3];
			points->GetPoint(e.vertex1(), tmpv);
			for (int i = 0; i < 3; ++i) {
				centerv[i] += tmpv[i];
			}
			points->GetPoint(e.vertex2(), tmpv);
			for (int i = 0; i < 3; ++i) {
				centerv[i] += tmpv[i];
			}
			pnum += 2;
		}
	}

	if (pnum == 0) {
		return;
	}

	for (int i = 0; i < 3; ++i) {
		centerv[i] /= pnum;
	}
	actor2DCollection()->AddItem(impl->m_nameActor);
	vtkCoordinate* coord = impl->m_nameActor->GetPositionCoordinate();
	coord->SetValue(centerv[0], centerv[1], centerv[2]);
	impl->m_nameMapper->SetInput(impl->m_inputGridBC->caption().c_str());

	auto tprop = impl->m_nameMapper->GetTextProperty();

	QColor color = impl->m_setting.color;
	tprop->SetColor(color.redF(), color.greenF(), color.blueF());
	impl->m_nameMapper->Modified();
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
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		for (auto index : impl->m_inputGridBC->indices()) {
			vtkCell* cell = grid->GetCell(index);
			ca->InsertNextCell(cell->GetPointIds());
		}
		d->SetPolys(ca);
	} else if (impl->m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
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

QDialog* Post2dWindowBCDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Display Setting"));
	dialog->resize(200, 100);

	return dialog;
}

void Post2dWindowBCDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
}

void Post2dWindowBCDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}