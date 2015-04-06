#include "preprocessorgriddataitem.h"
#include "preprocessorunstructured2dgridshapedataitem.h"
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <misc/xmlsupport.h>
#include <misc/stringtool.h>

#include <QXmlStreamWriter>
#include <vtkCollectionIterator.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor2DCollection.h>
#include <vtkPointData.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>

PreProcessorUnstructured2dGridShapeDataItem::PreProcessorUnstructured2dGridShapeDataItem(PreProcessorDataItem* parent)
	: PreProcessorGridShapeDataItem(parent)
{
	setupActors();
	updateActorSettings();
}

PreProcessorUnstructured2dGridShapeDataItem::~PreProcessorUnstructured2dGridShapeDataItem()
{
	renderer()->RemoveActor(m_wireframeActor);
	renderer()->RemoveActor2D(m_indexActor);
}

void PreProcessorUnstructured2dGridShapeDataItem::setupActors()
{
	m_wireframeActor = vtkSmartPointer<vtkActor>::New();
	m_wireframeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_wireframeActor);

	m_wireframeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_wireframeMapper->SetScalarVisibility(false);
	m_wireframeMapper->ImmediateModeRenderingOn();
	m_wireframeActor->SetMapper(m_wireframeMapper);
	m_wireframeActor->GetProperty()->SetRepresentationToWireframe();

	m_indexActor = vtkSmartPointer<vtkActor2D>::New();
	renderer()->AddActor2D(m_indexActor);

	m_indexMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	m_indexMapper->SetLabelModeToLabelFieldData();
	m_indexMapper->SetFieldDataName(Grid::LABEL_NAME);
	m_indexMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
	m_indexMapper->GetLabelTextProperty()->SetFontSize(12);
	m_indexMapper->GetLabelTextProperty()->BoldOff();
	m_indexMapper->GetLabelTextProperty()->ItalicOff();
	m_indexMapper->GetLabelTextProperty()->ShadowOff();
	m_indexMapper->GetLabelTextProperty()->SetJustificationToLeft();
	m_indexMapper->GetLabelTextProperty()->SetVerticalJustificationToCentered();

	m_indexActor->SetMapper(m_indexMapper);

	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
}


void PreProcessorUnstructured2dGridShapeDataItem::informGridUpdate()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != 0){
		Unstructured2DGrid* grid = dynamic_cast<Unstructured2DGrid*>(g);

		m_wireframeMapper->SetInputData(grid->vtkFilteredShape());
		m_indexMapper->SetInputData(grid->vtkFilteredIndexGrid());
	}
	updateActorSettings();
}

void PreProcessorUnstructured2dGridShapeDataItem::updateActorSettings()
{
	// make all the items invisible
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
//	m_axesActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == 0){
		// grid is not setup yet.
		return;
	}
	m_wireframeActor->GetProperty()->SetColor(m_color);
	m_actorCollection->AddItem(m_wireframeActor);

	if (m_indexVisible){
		m_indexMapper->GetLabelTextProperty()->SetColor(m_indexColor);
		m_actor2DCollection->AddItem(m_indexActor);
	}
/*
	if (m_axesVisible){
		m_axesActor->GetProperty()->SetColor(m_axesColor);
		m_axesActor->GetAxisLabelTextProperty()->SetColor(m_axesColor);
		m_axesActor->GetAxisTitleTextProperty()->SetColor(m_axesColor);
		m_actor2DCollection->AddItem(m_axesActor);
		updateAxesRegion();
	}
*/
	updateVisibilityWithoutRendering();
}

QDialog* PreProcessorUnstructured2dGridShapeDataItem::propertyDialog(QWidget* parent)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(parent);
	dialog->hideShape();
	dialog->setGridColor(color());
	dialog->setShape(GridShapeEditDialog::Wireframe);
	dialog->setIndexVisible(m_indexVisible);
	dialog->setIndexColor(indexColor());
//	dialog->setAxesVisible(m_axesVisible);
//	dialog->setAxesColor(axesColor());
	return dialog;
}

void PreProcessorUnstructured2dGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	setColor(dialog->gridColor());
	m_indexVisible = dialog->indexVisible();
	setIndexColor(dialog->indexColor());
//	m_axesVisible = dialog->axesVisible();
//	setAxesColor(dialog->axesColor());
	updateActorSettings();
	renderGraphicsView();
}

void PreProcessorUnstructured2dGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent())->setSelectedPointsVisibility(true);
	updateVisibility();
}

void PreProcessorUnstructured2dGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent())->setSelectedPointsVisibility(false);
}

void PreProcessorUnstructured2dGridShapeDataItem::assignActionZValues(const ZDepthRange& range)
{
	m_wireframeActor->SetPosition(0, 0, range.min());
//	m_axesActor->SetPosition(0, 0, range.max());
}
