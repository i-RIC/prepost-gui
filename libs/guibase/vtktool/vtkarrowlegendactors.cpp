#include "vtkarrowlegendactors.h"
#include "private/vtkarrowlegendactors_impl.h"

#include <misc/stringtool.h>

#include <QString>

#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <cmath>

vtkArrowLegendActors::vtkArrowLegendActors() :
	impl {new Impl {}}
{}

vtkArrowLegendActors::~vtkArrowLegendActors()
{
	delete impl;
}

vtkArrowLegendActors::Impl::Impl() :
	m_nameTextActor {vtkTextActor::New()},
	m_valueTextActor {vtkTextActor::New()},
	m_arrowActor {vtkActor2D::New()},
	m_mapper {vtkPolyDataMapper2D::New()}
{
	m_nameTextActor->SetTextScaleModeToNone();
	m_nameTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();

	m_valueTextActor->SetTextScaleModeToNone();
	m_valueTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();

	m_arrowActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();

	auto prop = m_nameTextActor->GetTextProperty();
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToBottom();

	prop = m_valueTextActor->GetTextProperty();
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToTop();

	m_arrowActor->SetMapper(m_mapper);
}

vtkArrowLegendActors::Impl::~Impl()
{
	m_nameTextActor->Delete();
	m_valueTextActor->Delete();
	m_arrowActor->Delete();
	m_mapper->Delete();
}

void vtkArrowLegendActors::update(const std::string& name, double pixels, double length, double headLen, double angle)
{
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	auto lines = vtkSmartPointer<vtkCellArray>::New();
	auto polys = vtkSmartPointer<vtkCellArray>::New();

	// add line
	vtkIdType ids[3];
	points->InsertNextPoint(-pixels * 0.5, 0, 0);
	points->InsertNextPoint( pixels * 0.5, 0, 0);

	ids[0] = 0; ids[1] = 1;
	lines->InsertNextCell(2, ids);

	// add triangle
	double yoffset = headLen * std::sin(angle / 180. * 3.141592);
	points->InsertNextPoint(pixels * .5 - headLen, yoffset, 0);
	points->InsertNextPoint(pixels * .5 - headLen, - yoffset, 0);
	ids[0] = 1;
	ids[1] = 2;
	ids[2] = 3;
	polys->InsertNextCell(3, ids);

	polyData->SetPoints(points);
	polyData->SetLines(lines);
	polyData->SetPolys(polys);

	impl->m_mapper->SetInputData(polyData);

	impl->m_nameTextActor->SetInput(name.c_str());
	impl->m_valueTextActor->SetInput(iRIC::toStr(QString("%1").arg(length)).c_str());
}

void vtkArrowLegendActors::setPosition(double xpos, double ypos)
{
	impl->m_arrowActor->GetPositionCoordinate()->SetValue(xpos, ypos);
	impl->m_nameTextActor->SetPosition(xpos, ypos + 0.01);
	impl->m_valueTextActor->SetPosition(xpos, ypos - 0.01);
}

void vtkArrowLegendActors::setColor(double r, double g, double b)
{
	impl->m_arrowActor->GetProperty()->SetColor(r, g, b);
}

void vtkArrowLegendActors::setColor(double* rgb)
{
	impl->m_arrowActor->GetProperty()->SetColor(rgb);
}

void vtkArrowLegendActors::setLineWidth(double lineWidth)
{
	impl->m_arrowActor->GetProperty()->SetLineWidth(lineWidth);
}

vtkTextActor* vtkArrowLegendActors::nameTextActor() const
{
	return impl->m_nameTextActor;
}

vtkTextActor* vtkArrowLegendActors::valueTextActor() const
{
	return impl->m_valueTextActor;
}

vtkActor2D* vtkArrowLegendActors::arrowActor() const
{
	return impl->m_arrowActor;
}
