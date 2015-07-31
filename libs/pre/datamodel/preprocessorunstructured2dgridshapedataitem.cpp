#include "preprocessorgriddataitem.h"
#include "preprocessorunstructured2dgridshapedataitem.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkCollectionIterator.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PreProcessorUnstructured2dGridShapeDataItem::PreProcessorUnstructured2dGridShapeDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridShapeDataItem {parent}
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
	vtkTextProperty* prop = m_indexMapper->GetLabelTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontSize(12);
	prop->BoldOff();
	prop->ItalicOff();
	prop->ShadowOff();
	prop->SetJustificationToLeft();
	prop->SetVerticalJustificationToCentered();

	m_indexActor->SetMapper(m_indexMapper);

	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
}


void PreProcessorUnstructured2dGridShapeDataItem::informGridUpdate()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != nullptr) {
		Unstructured2DGrid* grid = dynamic_cast<Unstructured2DGrid*>(g);

		vtkAlgorithm* shapeAlgo = grid->vtkFilteredShapeAlgorithm();
		if (shapeAlgo != nullptr) {m_wireframeMapper->SetInputConnection(shapeAlgo->GetOutputPort());}
		vtkAlgorithm* indexGridAlgo = grid->vtkFilteredIndexGridAlgorithm();
		if (indexGridAlgo != nullptr) {m_indexMapper->SetInputConnection(indexGridAlgo->GetOutputPort());}
	}
	updateActorSettings();
}

void PreProcessorUnstructured2dGridShapeDataItem::updateActorSettings()
{
	// make all the items invisible
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == nullptr) {
		// grid is not setup yet.
		return;
	}
	m_wireframeActor->GetProperty()->SetColor(m_setting.color);
	m_actorCollection->AddItem(m_wireframeActor);

	if (m_setting.indexVisible) {
		m_indexMapper->GetLabelTextProperty()->SetColor(m_setting.indexColor);
		m_actor2DCollection->AddItem(m_indexActor);
	}
	updateVisibilityWithoutRendering();
}

QDialog* PreProcessorUnstructured2dGridShapeDataItem::propertyDialog(QWidget* parent)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(parent);
	dialog->hideShape();
	dialog->setSetting(m_setting);
	return dialog;
}

void PreProcessorUnstructured2dGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	m_setting = dialog->setting();

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

void PreProcessorUnstructured2dGridShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_wireframeActor->SetPosition(0, 0, range.min());
}
