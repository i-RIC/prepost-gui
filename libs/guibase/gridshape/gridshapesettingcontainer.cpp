#include "gridshapesettingcontainer.h"

#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkLabeledDataMapper.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

#include <QSettings>

GridShapeSettingContainer::GridShapeSettingContainer() :
	CompositeContainer ({&color, &shape, &indexVisible, &indexTextSetting}),
	color {"color", Qt::black},
	shape {"shape", Shape::Outline},
	indexVisible {"indexVisible", false},
	indexTextSetting {},
	m_outlineActor {vtkActor::New()},
	m_wireframeActor {vtkActor::New()},
	m_indexActor {vtkActor2D::New()},
	m_transform {vtkTransformFilter::New()}
{
	indexTextSetting.addPrefix("indexTextSetting");
	indexTextSetting.fontSize = 15;

	QSettings setting;
	color = setting.value("graphics/gridcolor", QColor(Qt::black)).value<QColor>();
	indexTextSetting.loadSetting(setting, "fontsetting/gridindices");

	m_outlineActor->GetProperty()->SetLighting(false);

	m_wireframeActor->GetProperty()->SetLighting(false);
	m_wireframeActor->GetProperty()->SetRepresentationToWireframe();

	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();

	auto t = vtkSmartPointer<vtkTransform>::New();
	m_transform->SetTransform(t);
}

GridShapeSettingContainer::GridShapeSettingContainer(const GridShapeSettingContainer& setting) :
	GridShapeSettingContainer {}
{
	copyValue(setting);
}

GridShapeSettingContainer::~GridShapeSettingContainer()
{
	m_outlineActor->Delete();
	m_wireframeActor->Delete();
	m_indexActor->Delete();
	m_transform->Delete();
}

GridShapeSettingContainer& GridShapeSettingContainer::operator=(const GridShapeSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& GridShapeSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const GridShapeSettingContainer&> (c));
}

vtkActor* GridShapeSettingContainer::outlineActor() const
{
	return m_outlineActor;
}

vtkActor* GridShapeSettingContainer::wireframeActor() const
{
	return m_wireframeActor;
}

vtkActor2D* GridShapeSettingContainer::indexActor() const
{
	return m_indexActor;
}

void GridShapeSettingContainer::update(vtkActorCollection* col, vtkActor2DCollection* col2d,
																			 vtkPointSet* outlineGrid, vtkPointSet* wireframeGrid,
																			 vtkPointSet* labelGrid, const std::string& labelName)
{
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();

	col->RemoveAllItems();
	col2d->RemoveAllItems();

	auto sgrid = vtkStructuredGrid::SafeDownCast(outlineGrid);
	if (sgrid != nullptr) {
		if (shape == Shape::Wireframe) {
			if (wireframeGrid != nullptr) {
				updateWireframeActor(wireframeGrid);
				col->AddItem(m_wireframeActor);
			}
		}
		if (outlineGrid != nullptr) {
			updateOutlineActor(sgrid);
			col->AddItem(m_outlineActor);
		}
	} else {
		if (wireframeGrid != nullptr) {
			updateWireframeActor(wireframeGrid);
			col->AddItem(m_wireframeActor);
		}
	}

	if (indexVisible && labelGrid != nullptr) {
		updateIndexActor(labelGrid, labelName);
		col2d->AddItem(m_indexActor);
	}
}

void GridShapeSettingContainer::setScale(double xScale, double yScale, double zScale)
{
	m_outlineActor->SetScale(xScale, yScale, zScale);
	m_wireframeActor->SetScale(xScale, yScale, zScale);
	auto t = vtkSmartPointer<vtkTransform>::New();
	t->Scale(xScale, yScale, zScale);
	m_transform->SetTransform(t);
}

void GridShapeSettingContainer::updateOutlineActor(vtkStructuredGrid* grid)
{
	auto filter = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
	filter->SetInputData(grid);

	auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(filter->GetOutputPort());

	m_outlineActor->SetMapper(mapper);
	m_outlineActor->GetProperty()->SetColor(color);
}

void GridShapeSettingContainer::updateWireframeActor(vtkPointSet* grid)
{
	auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->ScalarVisibilityOff();
	mapper->SetInputData(grid);

	m_wireframeActor->SetMapper(mapper);
	m_wireframeActor->GetProperty()->SetColor(color);
}

void GridShapeSettingContainer::updateIndexActor(vtkPointSet* grid, const std::string& labelName)
{
	auto mapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	indexTextSetting.applySetting(mapper->GetLabelTextProperty());

	m_transform->SetInputData(grid);

	mapper->SetInputConnection(m_transform->GetOutputPort());
	mapper->SetLabelModeToLabelFieldData();
	mapper->SetFieldDataName(labelName.c_str());

	m_indexActor->SetMapper(mapper);
}
