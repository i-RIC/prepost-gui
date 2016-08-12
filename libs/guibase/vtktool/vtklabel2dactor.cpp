#include "vtklabel2dactor.h"
#include "private/vtklabel2dactor_impl.h"

#include <misc/stringtool.h>

#include <QString>

#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkStringArray.h>
#include <vtkTextProperty.h>

#define LABEL "Label"

vtkLabel2DActor::Impl::Impl() :
	m_actor {vtkActor2D::New()},
	m_points {vtkPoints::New()},
	m_polyData {vtkPolyData::New()},
	m_mapper {vtkLabeledDataMapper::New()},
	m_stringArray {vtkStringArray::New()},
	m_position {},
	m_labelPosition {lpBottomCenter},
	m_label ("")
{
	m_points->SetDataTypeToDouble();
	m_points->InsertNextPoint(0, 0, 0);
	m_polyData->SetPoints(m_points);

	vtkIdType cellId = 0;
	vtkCellArray* ca = vtkCellArray::New();
	ca->InsertNextCell(1, &cellId);
	m_polyData->SetVerts(ca);
	ca->Delete();

	m_stringArray->SetName(LABEL);
	m_stringArray->InsertNextValue("");
	m_polyData->GetPointData()->AddArray(m_stringArray);

	m_mapper->SetLabelModeToLabelFieldData();
	m_mapper->SetFieldDataName(LABEL);
	m_mapper->SetInputData(m_polyData);

	vtkTextProperty* txtprop = m_mapper->GetLabelTextProperty();
	txtprop->SetColor(0, 0, 0);
	txtprop->SetFontSize(10);
	txtprop->BoldOff();
	txtprop->ItalicOff();
	txtprop->ShadowOff();

	m_actor->SetMapper(m_mapper);

	setup();
}

vtkLabel2DActor::Impl::~Impl()
{
	m_actor->Delete();
	m_points->Delete();
	m_polyData->Delete();
	m_mapper->Delete();
	m_stringArray->Delete();
}

void vtkLabel2DActor::Impl::setup()
{
	QString label;

	m_points->SetPoint(0, m_position.x(), m_position.y(), 0);
	m_points->Modified();

	label = m_label.c_str();

	vtkTextProperty* txtProp = m_mapper->GetLabelTextProperty();

	switch (m_labelPosition) {
	case lpTopCenter:
		txtProp->SetJustificationToCentered();
		txtProp->SetVerticalJustificationToBottom();
		txtProp->SetLineOffset(-5);
		break;
	case lpBottomCenter:
		txtProp->SetJustificationToCentered();
		txtProp->SetVerticalJustificationToTop();
		txtProp->SetLineOffset(5);
		break;
	case lpMiddleLeft:
		txtProp->SetJustificationToRight();
		txtProp->SetVerticalJustificationToCentered();
		txtProp->SetLineOffset(0);
		label = label.append("   ");
		break;
	case lpMiddleRight:
		txtProp->SetJustificationToLeft();
		txtProp->SetVerticalJustificationToCentered();
		txtProp->SetLineOffset(0);
		label = QString("   ").append(label);
		break;
	}
	m_stringArray->SetValue(0, iRIC::toStr(label).c_str());
	m_stringArray->Modified();
}

vtkLabel2DActor::vtkLabel2DActor() :
	impl {new Impl()}
{}

vtkLabel2DActor::~vtkLabel2DActor()
{
	delete impl;
}

vtkActor2D* vtkLabel2DActor::actor() const
{
	return impl->m_actor;
}

vtkTextProperty* vtkLabel2DActor::labelTextProperty() const
{
	return impl->m_mapper->GetLabelTextProperty();
}

QPointF vtkLabel2DActor::position() const
{
	return impl->m_position;
}

void vtkLabel2DActor::setPosition(const QPointF& position)
{
	impl->m_position = position;
	impl->setup();
}

vtkLabel2DActor::LabelPosition vtkLabel2DActor::labelPosition() const
{
	return impl->m_labelPosition;
}

void vtkLabel2DActor::setLabelPosition(LabelPosition position)
{
	impl->m_labelPosition = position;
	impl->setup();
}

std::string vtkLabel2DActor::label() const
{
	return impl->m_label;
}

void vtkLabel2DActor::setLabel(const std::string& label)
{
	impl->m_label = label;
	impl->setup();
}
