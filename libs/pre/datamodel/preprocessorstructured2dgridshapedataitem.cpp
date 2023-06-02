#include "preprocessorgriddataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"

#include <guibase/graphicsmisc.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkAxisActor2D.h>
#include <vtkCell.h>
#include <vtkCollectionIterator.h>
#include <vtkGeometryFilter.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PreProcessorStructured2dGridShapeDataItem::PreProcessorStructured2dGridShapeDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridShapeDataItem {parent}
{
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

	auto textProp = m_indexMapper->GetLabelTextProperty();
	iRIC::setupGridIndexTextProperty(textProp);
	m_setting.indexTextSetting.applySetting(textProp);

	m_indexActor->SetMapper(m_indexMapper);

	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();
}

void PreProcessorStructured2dGridShapeDataItem::informGridUpdate()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != nullptr) {
		auto grid = dynamic_cast<Structured2DGrid*>(g);
		m_outlineFilter->SetInputData(grid->vtkGrid());

		auto filteredGrid = g->vtkFilteredGrid();
		if (filteredGrid != nullptr) {
			auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
			filter->SetInputData(filteredGrid);
			m_wireframeMapper->SetInputConnection(filter->GetOutputPort());
		}
		
		auto filteredIndexGrid = g->vtkFilteredIndexGrid();
		if (filteredIndexGrid != nullptr) {m_indexMapper->SetInputData(filteredIndexGrid);}
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

	switch (GridShapeEditDialog::Shape(m_setting.shape)) {
	case GridShapeEditDialog::Outline:
		m_outlineActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_outlineActor);
		break;

	case GridShapeEditDialog::Wireframe:
		m_outlineActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_outlineActor);
		m_wireframeActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_wireframeActor);
		break;
	}

	if (m_setting.indexVisible) {
		m_setting.indexTextSetting.applySetting(m_indexMapper->GetLabelTextProperty());
		m_actor2DCollection->AddItem(m_indexActor);
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorStructured2dGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	PreProcessorGridShapeDataItem::doLoadFromProjectMainFile(node);
}

void PreProcessorStructured2dGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	PreProcessorGridShapeDataItem::doSaveToProjectMainFile(writer);
}

QDialog* PreProcessorStructured2dGridShapeDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new GridShapeEditDialog(parent);
	dialog->setSetting(m_setting);
	return dialog;
}

void PreProcessorStructured2dGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	m_setting = dialog->setting();
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

void PreProcessorStructured2dGridShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_outlineActor->SetPosition(0, 0, range.max());
	m_wireframeActor->SetPosition(0, 0, range.min());
}
