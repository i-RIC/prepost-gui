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
	m_textActor {vtkTextActor::New()},
	m_arrowActor {vtkActor2D::New()},
	m_mapper {vtkPolyDataMapper2D::New()}
{
	m_textActor->SetTextScaleModeToNone();
	m_textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();

	m_arrowActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();

	auto prop = m_textActor->GetTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontFamilyToArial();
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToBottom();

	m_arrowActor->SetMapper(m_mapper);
}

vtkArrowLegendActors::Impl::~Impl()
{
	m_textActor->Delete();
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

	double vOffset = 18;

	// add line
	vtkIdType ids[3];
	points->InsertNextPoint(-pixels * 0.5, vOffset, 0);
	points->InsertNextPoint( pixels * 0.5, vOffset, 0);

	ids[0] = 0; ids[1] = 1;
	lines->InsertNextCell(2, ids);

	// add triangle
	double yoffset = headLen * std::sin(angle / 180. * 3.141592);
	points->InsertNextPoint(pixels * .5 - headLen, vOffset + yoffset, 0);
	points->InsertNextPoint(pixels * .5 - headLen, vOffset - yoffset, 0);
	ids[0] = 1;
	ids[1] = 2;
	ids[2] = 3;
	polys->InsertNextCell(3, ids);

	polyData->SetPoints(points);
	polyData->SetLines(lines);
	polyData->SetPolys(polys);

	impl->m_mapper->SetInputData(polyData);

	auto legendStr = QString("%1\n\n%2").arg(name.c_str()).arg(length);
	impl->m_textActor->SetInput(iRIC::toStr(legendStr).c_str());
}

void vtkArrowLegendActors::setPosition(double xpos, double ypos)
{
	impl->m_arrowActor->GetPositionCoordinate()->SetValue(xpos, ypos);
	impl->m_textActor->SetPosition(xpos, ypos);
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

vtkTextActor* vtkArrowLegendActors::textActor() const
{
	return impl->m_textActor;
}

vtkActor2D* vtkArrowLegendActors::arrowActor() const
{
	return impl->m_arrowActor;
}
