#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridtypedataitem.h"
#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowaxessettingdialog.h"
#include "gridbirdeyewindowcolorsettingdialog.h"
#include "gridbirdeyewindowdatamodel.h"
#include "gridbirdeyewindowgraphicsview.h"

#include <guibase/graphicsmisc.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QInputDialog>
#include <QUndoCommand>

#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>

#define ELEVATION "Elevation"

GridBirdEyeWindowDataModel::GridBirdEyeWindowDataModel(GridBirdEyeWindow* w, ProjectDataItem* parent)
	: GraphicsWindowSimpleDataModel(w, parent)
{
	init();
	updateGrid();
	setDefaultColor();
	setAxesColor(Qt::black);
	updateColor();
}

GridBirdEyeWindowDataModel::~GridBirdEyeWindowDataModel()
{

}

GridBirdEyeWindowGraphicsView* GridBirdEyeWindowDataModel::graphicsView() const
{
	return dynamic_cast<GridBirdEyeWindowGraphicsView*>(m_graphicsView);
}

void GridBirdEyeWindowDataModel::init()
{
	m_graphicsView->setModel(this);
	// add actor to show grid.
	m_actor = vtkSmartPointer<vtkActor>::New();

	m_mapper = vtkSmartPointer<vtkDataSetMapper>::New();

	m_warp = vtkSmartPointer<vtkWarpScalar>::New();
	m_warp->SetUseNormal(1);
	m_warp->SetNormal(0, 0, 1);
	m_warp->SetScaleFactor(DEFAULT_SCALE);
	m_warp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, ELEVATION);

	vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
	light->SetLightTypeToCameraLight();
	light->SetPosition(0, 1, 0);
	light->SetAmbientColor(0.2, 0.2, 0.2);
	light->SetColor(0.2, 0.2, 0.2);

	light->SetFocalPoint(0, 0, 0);
	m_graphicsView->mainRenderer()->AddLight(light);

	light = vtkSmartPointer<vtkLight>::New();
	light->SetLightTypeToHeadlight();
	m_graphicsView->mainRenderer()->AddLight(light);

	m_mapper->SetInputConnection(m_warp->GetOutputPort());
	m_actor->SetMapper(m_mapper);

	m_cubeAxesActor = vtkSmartPointer<vtkCubeAxesActor2D>::New();
	iRIC::setupCubeActor2D(m_cubeAxesActor);
	m_cubeAxesActor->SetZAxisVisibility(0);

	m_cubeAxesActor->SetCamera(m_graphicsView->mainRenderer()->GetActiveCamera());

	m_graphicsView->mainRenderer()->AddActor(m_actor);
	m_graphicsView->mainRenderer()->AddActor2D(m_cubeAxesActor);

	vtkRenderWindowInteractor* iren = m_graphicsView->mainRenderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetEnabled(0);
	m_scalarBarWidget->SetInteractor(iren);

	m_axesActor = vtkSmartPointer<vtkAxesActor>::New();
	iRIC::setupAxesActor(m_axesActor);
	m_axesActor->SetTotalLength(1, 1, 1);

	m_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	m_widget->SetOutlineColor(.5, .5, .5);
	m_widget->SetOrientationMarker(m_axesActor);
	m_widget->SetInteractor(iren);
	m_widget->SetViewport(0.0, 0.0, 0.2, 0.2);
	m_widget->SetEnabled(1);
	m_widget->InteractiveOn();
}

void GridBirdEyeWindowDataModel::setDefaultColor()
{
	PreProcessorGridDataItem* dItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* g = dItem->grid();

	m_customColor = Qt::blue;
	if (g->gridRelatedCondition(ELEVATION) != nullptr) {
		m_colorType = ctNode;
		m_attributeName = ELEVATION;
	} else {
		m_colorType = ctCustom;
	}
}

class GridBirdEyeWindowEditZScaleCommand : public QUndoCommand
{
public:
	GridBirdEyeWindowEditZScaleCommand(double oldScale, double newScale, GridBirdEyeWindowDataModel* m)
		: QUndoCommand(GridBirdEyeWindowDataModel::tr("Edit Z-direction Scale")) {
		m_oldScale = oldScale;
		m_newScale = newScale;
		m_model = m;
	}
	void undo() {
		m_model->m_warp->SetScaleFactor(m_oldScale);
		m_model->updateAxes();
		m_model->graphicsView()->cameraFit();
	}
	void redo() {
		m_model->m_warp->SetScaleFactor(m_newScale);
		m_model->updateAxes();
		m_model->graphicsView()->cameraFit();
	}
private:
	double m_oldScale;
	double m_newScale;
	GridBirdEyeWindowDataModel* m_model;
};

void GridBirdEyeWindowDataModel::editZScale()
{
	double current = m_warp->GetScaleFactor();
	bool ok;
	QWidget* p = mainWindow();
	double newZscale = QInputDialog::getDouble(p, tr("Z-direction Scale"), tr("Input new Z-direction scale."), current, 1E-3, 1E3, 3, &ok);
	if (! ok) {return;}
	iRICUndoStack::instance().push(new GridBirdEyeWindowEditZScaleCommand(current, newZscale, this));
}

class GridBirdEyeWindowEditColorCommand : public QUndoCommand
{
public:
	GridBirdEyeWindowEditColorCommand(GridBirdEyeWindowDataModel::ColorType ct, QString attName, QColor customColor, bool axesVisible, QColor axesColor, GridBirdEyeWindowDataModel* m)
		: QUndoCommand(GridBirdEyeWindowDataModel::tr("Edit Color Setting")) {
		m_model = m;

		m_newColorType = ct;
		m_newAttName = attName;
		m_newCustomColor = customColor;
		m_newAxesVisible = axesVisible;
		m_newAxesColor = axesColor;

		m_oldColorType = m_model->m_colorType;
		m_oldAttName = m_model->m_attributeName;
		m_oldCustomColor = m_model->m_customColor;
		m_oldAxesVisible = (m_model->m_cubeAxesActor->GetVisibility() == 1);
		m_oldAxesColor = m_model->axesColor();
	}
	void undo() {
		m_model->m_colorType = m_oldColorType;
		m_model->m_attributeName = m_oldAttName;
		m_model->m_customColor = m_oldCustomColor;
		m_model->m_cubeAxesActor->SetVisibility(m_oldAxesVisible);
		m_model->setAxesColor(m_oldAxesColor);
		m_model->m_widget->SetEnabled(m_oldAxesVisible);

		m_model->updateColor();
	}
	void redo() {
		m_model->m_colorType = m_newColorType;
		m_model->m_attributeName = m_newAttName;
		m_model->m_customColor = m_newCustomColor;
		m_model->m_cubeAxesActor->SetVisibility(m_newAxesVisible);
		m_model->setAxesColor(m_newAxesColor);
		m_model->m_widget->SetEnabled(m_newAxesVisible);

		m_model->updateColor();
	}
private:
	GridBirdEyeWindowDataModel::ColorType m_oldColorType;
	QString m_oldAttName;
	QColor m_oldCustomColor;
	bool m_oldAxesVisible;
	QColor m_oldAxesColor;

	GridBirdEyeWindowDataModel::ColorType m_newColorType;
	QString m_newAttName;
	QColor m_newCustomColor;
	bool m_newAxesVisible;
	QColor m_newAxesColor;

	GridBirdEyeWindowDataModel* m_model;
};

void GridBirdEyeWindowDataModel::displaySetting()
{
	GridBirdEyeWindowColorSettingDialog dialog(mainWindow());
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(gItem->parent()->parent());
	dialog.setGridType(gtItem->gridType());
	dialog.setColorType(m_colorType);
	dialog.setAttributeName(m_attributeName);
	dialog.setCustomColor(m_customColor);
	dialog.setAxesVisible(m_cubeAxesActor->GetVisibility() == 1);
	dialog.setAxisColor(axesColor());

	int ret = dialog.exec();

	if (ret != QDialog::Accepted) {return;}
	iRICUndoStack::instance().push(new GridBirdEyeWindowEditColorCommand(dialog.colorType(), dialog.attributeName(), dialog.customColor(), dialog.axesVisible(), dialog.axesColor(), this));
}

void GridBirdEyeWindowDataModel::updateGrid()
{
	PreProcessorGridDataItem* dItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* g = dItem->grid();

	m_warp->SetInputData(g->vtkGrid());
	updateAxes();
}

void GridBirdEyeWindowDataModel::updateAxes()
{
	double bounds[6];
	m_warp->Update();
	m_warp->GetOutput()->GetBounds(bounds);

	iRIC::fattenBounds(bounds, 0.1);
	m_cubeAxesActor->SetBounds(bounds);
}

void GridBirdEyeWindowDataModel::updateColor()
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(gItem->parent()->parent());
	switch (m_colorType) {
	case ctNode:
		m_mapper->SetScalarVisibility(1);
		m_mapper->SetScalarModeToUsePointFieldData();
		m_mapper->SetLookupTable(gtItem->scalarsToColors(m_attributeName)->vtkObj());
		m_mapper->UseLookupTableScalarRangeOn();
		m_mapper->SelectColorArray(iRIC::toStr(m_attributeName).c_str());
		m_actor->GetProperty()->SetInterpolationToGouraud();
		break;
	case ctCell:
		m_mapper->SetScalarVisibility(1);
		m_mapper->SetScalarModeToUseCellFieldData();
		m_mapper->SetLookupTable(gtItem->scalarsToColors(m_attributeName)->vtkObj());
		m_mapper->UseLookupTableScalarRangeOn();
		m_mapper->SelectColorArray(iRIC::toStr(m_attributeName).c_str());
		m_actor->GetProperty()->SetInterpolationToFlat();
		break;
	case ctCustom:
		m_mapper->SetScalarVisibility(0);
		m_actor->GetProperty()->SetColor(m_customColor.redF(), m_customColor.greenF(), m_customColor.blueF());
		break;
	}
	updateScalarBar();
	m_graphicsView->GetRenderWindow()->Render();
}

void GridBirdEyeWindowDataModel::updateScalarBar()
{
	m_scalarBarWidget->SetEnabled(0);
	if (m_colorType == ctCustom) {
		return;
	}
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(gItem->parent()->parent());
	SolverDefinitionGridAttribute* cond = gtItem->gridType()->gridRelatedCondition(m_attributeName);
	if (cond->isOption()) {
		// scalar bar is needless.
		return;
	}
	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetLookupTable(gtItem->scalarsToColors(m_attributeName)->vtkObj());
	m_scalarBarWidget->SetEnabled(1);
}

void GridBirdEyeWindowDataModel::setAxesColor(const QColor& col)
{
	m_cubeAxesActor->GetProperty()->SetColor(col.redF(), col.greenF(), col.blueF());
	m_cubeAxesActor->GetAxisLabelTextProperty()->SetColor(col.redF(), col.greenF(), col.blueF());
	m_cubeAxesActor->GetAxisTitleTextProperty()->SetColor(col.redF(), col.greenF(), col.blueF());
}

QColor GridBirdEyeWindowDataModel::axesColor() const
{
	double col[3];
	m_cubeAxesActor->GetProperty()->GetColor(col);
	return QColor(col[0] * 255, col[1] * 255, col[2] * 255);
}

void GridBirdEyeWindowDataModel::doLoadFromProjectMainFile(const QDomNode&)
{}

void GridBirdEyeWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
