#include "vtkarrowsactor.h"
#include "private/vtkarrowsactor_impl.h"

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkGlyph3D.h>
#include <vtkHedgeHog.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkWarpVector.h>

vtkArrowsActor::Impl::Impl() :
	m_actor {vtkActor::New()},
	m_mapper {vtkPolyDataMapper::New()},
	m_conesAndHedgeHog {vtkAppendPolyData::New()},
	m_coneSource {vtkConeSource::New()},
	m_glyph {vtkGlyph3D::New()},
	m_hedgeHog {vtkHedgeHog::New()},
	m_warpVector {vtkWarpVector::New()}
{
	m_glyph->SetScaleModeToDataScalingOff();
	m_glyph->SetVectorModeToUseVector();
	m_glyph->SetInputConnection(m_warpVector->GetOutputPort());
	m_glyph->SetSourceConnection(m_coneSource->GetOutputPort());

	m_conesAndHedgeHog->AddInputConnection(m_hedgeHog->GetOutputPort());
	m_conesAndHedgeHog->AddInputConnection(m_glyph->GetOutputPort());

	m_mapper->SetInputConnection(m_conesAndHedgeHog->GetOutputPort());
	m_actor->SetMapper(m_mapper);
}

vtkArrowsActor::Impl::~Impl()
{
	m_actor->Delete();
	m_conesAndHedgeHog->Delete();
	m_coneSource->Delete();
	m_glyph->Delete();
	m_hedgeHog->Delete();
	m_warpVector->Delete();
}

vtkArrowsActor::vtkArrowsActor() :
	impl {new Impl {}}
{
	setAngle(15);
	setConeHeight(5);
}

vtkArrowsActor::~vtkArrowsActor()
{
	delete impl;
}

void vtkArrowsActor::setPolyData(vtkPolyData* polyData)
{
	impl->m_hedgeHog->SetInputData(polyData);
	impl->m_warpVector->SetInputData(polyData);
}

void vtkArrowsActor::setAngle(double angle)
{
	impl->m_angle = angle;
	impl->m_coneSource->SetAngle(angle);
	impl->m_coneSource->Modified();
}

void vtkArrowsActor::setLineWidth(double width)
{
	impl->m_actor->GetProperty()->SetLineWidth(width);
}

void vtkArrowsActor::setConeHeight(double height)
{
	impl->m_coneSource->SetHeight(height);
	impl->m_coneSource->SetAngle(impl->m_angle);
	impl->m_coneSource->Modified();
}

void vtkArrowsActor::setScaleFactor(double scaleFactor)
{
	impl->m_hedgeHog->SetScaleFactor(scaleFactor);
	impl->m_hedgeHog->Modified();

	impl->m_warpVector->SetScaleFactor(scaleFactor);
	impl->m_warpVector->Modified();
}

void vtkArrowsActor::setColorModeToCustom(double r, double g, double b)
{
	impl->m_mapper->ScalarVisibilityOff();
	impl->m_actor->GetProperty()->SetColor(r, g, b);
}

void vtkArrowsActor::setColorModeToCustom(double* rgb)
{
	impl->m_mapper->ScalarVisibilityOff();
	impl->m_actor->GetProperty()->SetColor(rgb);
}

void vtkArrowsActor::setColorModeToValue(const std::string& target, vtkScalarsToColors* scalarsToColors)
{
	impl->m_mapper->ScalarVisibilityOn();
	impl->m_mapper->SetScalarModeToUsePointFieldData();
	impl->m_mapper->SelectColorArray(target.c_str());
	impl->m_mapper->SetLookupTable(scalarsToColors);
	impl->m_mapper->UseLookupTableScalarRangeOn();
}

vtkActor* vtkArrowsActor::actor() const
{
	return impl->m_actor;
}
