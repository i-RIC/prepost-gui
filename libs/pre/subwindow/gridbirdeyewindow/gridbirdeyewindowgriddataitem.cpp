#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridtypedataitem.h"
#include "gridbirdeyewindowdatamodel.h"
#include "gridbirdeyewindowgraphicsview.h"
#include "gridbirdeyewindowgriddataitem.h"
#include "private/gridbirdeyewindowgriddataitem_editdisplaysettingdialog.h"
#include "private/gridbirdeyewindowgriddataitem_updateactorsettingcommand.h"

#include <guibase/graphicsmisc.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

#include <vtkGeometryFilter.h>
#include <vtkSmartPointer.h>
#include <vtkWarpScalar.h>

namespace {
	std::string ELEVATION = "Elevation";
}

GridBirdEyeWindowGridDataItem::GridBirdEyeWindowGridDataItem(GridBirdEyeWindowDataModel* model) :
	GraphicsWindowDataItem {model},
	m_actor {vtkActor::New()},
	m_cubeAxesActor {vtkCubeAxesActor2D::New()},
	m_orientationMarkerWidget {vtkOrientationMarkerWidget::New()},
	m_previousZScale {-1},
	m_model {model}
{
	setupActors();
	setDefaultSetting();
	setupLegendSettings();
	updateActorSettings();
}

GridBirdEyeWindowGridDataItem::~GridBirdEyeWindowGridDataItem()
{
	for (const auto& pair : m_colorMapLegendSettings) {
		delete pair.second;
	}

	auto r = m_model->graphicsView()->mainRenderer();

	for (const auto& pair : m_colorMapLegendActors) {
		r->RemoveActor2D(pair.second);
		pair.second->Delete();
	}

	r->RemoveActor(m_actor);
	m_actor->Delete();

	r->RemoveActor(m_cubeAxesActor);
	m_cubeAxesActor->Delete();

	m_orientationMarkerWidget->SetInteractor(nullptr);
	m_orientationMarkerWidget->Delete();
}

void GridBirdEyeWindowGridDataItem::editZScale()
{
	bool ok;
	QWidget* p = m_model->mainWindow();

	DoubleContainer newZScale("zScale");
	newZScale = QInputDialog::getDouble(p, tr("Edit Z-direction Scale"), tr("Input new Z-direction scale."), m_setting.zScale, 1E-3, 1E3, 3, &ok);
	if (! ok) {return;}

	pushUpdateActorSettingsCommand(new ValueModifyCommmand<DoubleContainer>("Edit Z Scale", newZScale, &m_setting.zScale));
}

void GridBirdEyeWindowGridDataItem::editDisplaySetting()
{
	iricMainWindow()->enterModelessDialogMode();

	auto dialog = new EditDisplaySettingDialog(this, m_model->mainWindow());
	connect(dialog, &QDialog::destroyed, [=](QObject*){iricMainWindow()->exitModelessDialogMode();});
	dialog->show();
}

void GridBirdEyeWindowGridDataItem::updateGrid()
{
	updateActorSettings();
}

void GridBirdEyeWindowGridDataItem::handleResize(VTKGraphicsView* v)
{
	auto s = activeLegendSetting();
	if (s == nullptr) {return;}

	s->imgSetting()->controller()->handleResize(v);
}

void GridBirdEyeWindowGridDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeLegendSetting();
	if (s == nullptr) {return;}

	s->imgSetting()->controller()->handleMouseMoveEvent(event, v);
}

void GridBirdEyeWindowGridDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeLegendSetting();
	if (s == nullptr) {return;}

	s->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void GridBirdEyeWindowGridDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeLegendSetting();
	if (s == nullptr) {return;}

	s->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
}

void GridBirdEyeWindowGridDataItem::setupActors()
{
	auto r = m_model->graphicsView()->mainRenderer();

	m_actor->VisibilityOff();
	r->AddActor(m_actor);

	iRIC::setupCubeActor2D(m_cubeAxesActor);
	r->AddActor(m_cubeAxesActor);
	m_cubeAxesActor->SetZAxisVisibility(0);
	m_cubeAxesActor->SetCamera(r->GetActiveCamera());

	auto axesActor = vtkSmartPointer<vtkAxesActor>::New();
	iRIC::setupAxesActor(axesActor);
	axesActor->SetTotalLength(1, 1, 1);
	m_orientationMarkerWidget->SetOrientationMarker(axesActor);

	auto interactor = r->GetRenderWindow()->GetInteractor();
	m_orientationMarkerWidget->SetInteractor(interactor);

	m_orientationMarkerWidget->SetOutlineColor(.5, .5, .5);
	m_orientationMarkerWidget->SetViewport(0, 0, 0.2, 0.2);
	m_orientationMarkerWidget->SetEnabled(1);
	m_orientationMarkerWidget->InteractiveOn();
}

void GridBirdEyeWindowGridDataItem::setDefaultSetting()
{
	auto dItem = gridDataItem();
	Grid* g = dItem->grid();

	if (g->gridAttribute(ELEVATION.c_str()) != nullptr) {
		m_setting.warpTarget = ELEVATION.c_str();
		m_setting.mapping = GridBirdEyeWindowSetting::Mapping::Value;
		m_setting.colorTarget = ELEVATION.c_str();
	} else {
		m_setting.mapping = GridBirdEyeWindowSetting::Mapping::Arbitrary;
	}
}

void GridBirdEyeWindowGridDataItem::setupLegendSettings()
{
	auto typeItem = gridTypeDataItem();
	auto r = m_model->graphicsView()->mainRenderer();

	for (const auto& pair : typeItem->colorMapSettings()) {
		const auto& name = pair.first;
		auto legend = pair.second->copyLegendSetting();
		legend->imgSetting()->controller()->setItem(this);
		m_colorMapLegendSettings.insert({name, legend});

		auto actor = vtkActor2D::New();
		legend->imgSetting()->setActor(actor);

		actor->VisibilityOff();
		r->AddActor2D(actor);
		m_colorMapLegendActors.insert({name, actor});
	}
}

void GridBirdEyeWindowGridDataItem::updateActorSettings()
{
	for (const auto& pair : m_colorMapLegendActors) {
		pair.second->VisibilityOff();
	}

	auto grid = gridDataItem()->grid();

	auto warp = vtkSmartPointer<vtkWarpScalar>::New();
	warp->SetInputData(grid->vtkGrid());
	warp->SetUseNormal(1);
	warp->SetNormal(0, 0, 1);
	warp->SetScaleFactor(m_setting.zScale);
	warp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_setting.warpTarget).c_str());

	auto geom = vtkSmartPointer<vtkGeometryFilter>::New();
	geom->SetInputConnection(warp->GetOutputPort());
	geom->Update();

	if (m_setting.mapping == GridBirdEyeWindowSetting::Mapping::Arbitrary) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputConnection(geom->GetOutputPort());
		m_actor->SetMapper(mapper);
		mapper->Delete();

		m_actor->GetProperty()->SetColor(m_setting.customColor);
	} else {
		auto colorTarget = iRIC::toStr(m_setting.colorTarget);
		auto ls = m_colorMapLegendSettings.at(colorTarget);

		auto gType = gridTypeDataItem();
		auto att = gType->gridType()->gridAttribute(colorTarget);
		if (att->position() == SolverDefinitionGridAttribute::Position::Node) {
			geom->GetOutput()->GetPointData()->SetActiveScalars(colorTarget.c_str());
			auto mapper = ls->setting()->buildPointDataMapper(geom->GetOutput());
			m_actor->SetMapper(mapper);
			mapper->Delete();
		} else {
			geom->GetOutput()->GetCellData()->SetActiveScalars(colorTarget.c_str());
			auto mapper = ls->setting()->buildCellDataMapper(geom->GetOutput(), false);
			m_actor->SetMapper(mapper);
			mapper->Delete();
		}
		ls->imgSetting()->controller()->handleSelection(m_model->graphicsView());
	}
	m_actor->VisibilityOn();

	if (m_setting.showAxes) {
		m_cubeAxesActor->VisibilityOn();
		m_cubeAxesActor->GetProperty()->SetColor(m_setting.axesColor);
		m_cubeAxesActor->GetAxisLabelTextProperty()->SetColor(m_setting.axesColor);
		m_cubeAxesActor->GetAxisTitleTextProperty()->SetColor(m_setting.axesColor);
	} else {
		m_cubeAxesActor->VisibilityOff();
	}

	double bounds[6];
	warp->GetOutput()->GetBounds(bounds);

	iRIC::fattenBounds(bounds, 0.1);
	m_cubeAxesActor->SetBounds(bounds);

	auto v = m_model->graphicsView();
	auto r = v->mainRenderer();
	r->SetBackground(m_setting.backgroundColor);

	if (m_previousZScale == m_setting.zScale) {
		v->render();
	} else {
		v->cameraFit();
	}

	m_previousZScale = m_setting.zScale;
}

ColorMapLegendSettingContainerI* GridBirdEyeWindowGridDataItem::activeLegendSetting() const
{
	if (m_setting.mapping == GridBirdEyeWindowSetting::Mapping::Arbitrary) {return nullptr;}

	auto it = m_colorMapLegendSettings.find(iRIC::toStr(m_setting.colorTarget));
	if (it == m_colorMapLegendSettings.end()) {return nullptr;}

	return it->second;
}

void GridBirdEyeWindowGridDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
}

void GridBirdEyeWindowGridDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

PreProcessorGridTypeDataItem* GridBirdEyeWindowGridDataItem::gridTypeDataItem() const
{
	return m_model->gridTypeDataItem();
}

PreProcessorGridDataItem* GridBirdEyeWindowGridDataItem::gridDataItem() const
{
	return m_model->gridDataItem();
}

GraphicsWindowDataModel* GridBirdEyeWindowGridDataItem::dataModel() const
{
	return reinterpret_cast<GraphicsWindowDataModel*> (m_model);
}

void GridBirdEyeWindowGridDataItem::pushUpdateActorSettingsCommand(QUndoCommand* command)
{
	iRICUndoStack::instance().push(new UpdateActorSettingCommand(command, this));
}
