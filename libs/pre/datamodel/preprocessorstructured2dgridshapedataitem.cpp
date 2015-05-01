#include "preprocessorgriddataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <misc/xmlsupport.h>
#include <misc/stringtool.h>

#include <QXmlStreamWriter>
#include <vtkActor2DCollection.h>
#include <vtkCollectionIterator.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkCell.h>
#include <vtkPointData.h>
#include <vtkTextProperty.h>
#include <vtkAxisActor2D.h>

PreProcessorStructured2dGridShapeDataItem::PreProcessorStructured2dGridShapeDataItem(PreProcessorDataItem* parent)
	: PreProcessorGridShapeDataItem(parent)
{
	m_shape = GridShapeEditDialog::Wireframe;
	m_indexVisible = false;
	setupActors();
	updateActorSettings();
}

PreProcessorStructured2dGridShapeDataItem::~PreProcessorStructured2dGridShapeDataItem()
{
	m_outlineFilter->SetInputData(0);

	renderer()->RemoveActor(m_outlineActor);
	renderer()->RemoveActor(m_wireframeActor);
	renderer()->RemoveActor2D(m_indexActor);
}

void PreProcessorStructured2dGridShapeDataItem::setupActors()
{
	m_outlineActor = vtkSmartPointer<vtkActor>::New();
	// no lighting is needed.
	m_outlineActor->GetProperty()->SetLighting(false);
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
	renderer()->AddActor(m_outlineActor);

	// build mapper too.
	m_outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_outlineActor->SetMapper(m_outlineMapper);

	// setup grid outline filter.
	m_outlineFilter = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
	m_outlineMapper->SetInputConnection(m_outlineFilter->GetOutputPort());

	m_wireframeActor = vtkSmartPointer<vtkActor>::New();
	m_wireframeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_wireframeActor);

	m_wireframeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_wireframeMapper->SetScalarVisibility(false);
	m_wireframeActor->SetMapper(m_wireframeMapper);
	m_wireframeActor->GetProperty()->SetRepresentationToWireframe();

	m_indexActor = vtkSmartPointer<vtkActor2D>::New();
	renderer()->AddActor2D(m_indexActor);

	m_indexMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	m_indexMapper->SetLabelModeToLabelFieldData();
	m_indexMapper->SetFieldDataName(Grid::LABEL_NAME);
	vtkTextProperty* textProp = m_indexMapper->GetLabelTextProperty();
	textProp->SetColor(0, 0, 0);
	textProp->SetFontSize(12);
	textProp->BoldOff();
	textProp->ItalicOff();
	textProp->ShadowOff();
	textProp->SetJustificationToLeft();
	textProp->SetVerticalJustificationToCentered();

	m_indexActor->SetMapper(m_indexMapper);

	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
}

void PreProcessorStructured2dGridShapeDataItem::informGridUpdate()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != nullptr) {
		Structured2DGrid* grid = dynamic_cast<Structured2DGrid*>(g);

		m_outlineFilter->SetInputData(grid->vtkGrid());
		vtkAlgorithm* shapeAlgo = grid->vtkFilteredShapeAlgorithm();
		if (shapeAlgo != nullptr) { m_wireframeMapper->SetInputConnection(shapeAlgo->GetOutputPort()); }
		vtkAlgorithm* indexGridAlgo = grid->vtkFilteredIndexGridAlgorithm();
		if (indexGridAlgo != nullptr) { m_indexMapper->SetInputConnection(indexGridAlgo->GetOutputPort()); }
	}
	updateActorSettings();
}

void PreProcessorStructured2dGridShapeDataItem::updateActorSettings()
{
	// make all the items invisible
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == nullptr) {
		// grid is not setup yet.
		return;
	}

	switch (m_shape) {
	case GridShapeEditDialog::Outline:
		m_outlineActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_outlineActor);
		break;

	case GridShapeEditDialog::Wireframe:
		m_outlineActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_outlineActor);
		m_wireframeActor->GetProperty()->SetColor(m_color);
		m_actorCollection->AddItem(m_wireframeActor);
		break;
	}

	if (m_indexVisible) {
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

void PreProcessorStructured2dGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{

	QDomElement elem = node.toElement();
	m_shape = (GridShapeEditDialog::Shape) elem.attribute("shape", "0").toInt();
	PreProcessorGridShapeDataItem::doLoadFromProjectMainFile(node);
}

void PreProcessorStructured2dGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// save the color and the shape of the grid
	writer.writeAttribute("shape", QString("%1").arg(m_shape));
	PreProcessorGridShapeDataItem::doSaveToProjectMainFile(writer);
}

QDialog* PreProcessorStructured2dGridShapeDataItem::propertyDialog(QWidget* parent)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(parent);
	dialog->setGridColor(color());
	dialog->setShape(shape());
	dialog->setIndexVisible(m_indexVisible);
	dialog->setIndexColor(indexColor());
//	dialog->setAxesVisible(m_axesVisible);
//	dialog->setAxesColor(axesColor());

	return dialog;
}

void PreProcessorStructured2dGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	m_shape = dialog->shape();
	setColor(dialog->gridColor());
	m_indexVisible = dialog->indexVisible();
	setIndexColor(dialog->indexColor());
//	m_axesVisible = dialog->axesVisible();
//	setAxesColor(dialog->axesColor());
	updateActorSettings();
	renderGraphicsView();
}

void PreProcessorStructured2dGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(selectedOutlineWidth);
	dynamic_cast<PreProcessorGridDataItem*>(parent())->setSelectedPointsVisibility(true);
	updateVisibility();
}

void PreProcessorStructured2dGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
	dynamic_cast<PreProcessorGridDataItem*>(parent())->setSelectedPointsVisibility(false);
}

void PreProcessorStructured2dGridShapeDataItem::assignActionZValues(const ZDepthRange& range)
{
	m_outlineActor->SetPosition(0, 0, range.max());
	m_wireframeActor->SetPosition(0, 0, range.min());
//	m_axesActor->SetPosition(0, 0, range.max());
}
