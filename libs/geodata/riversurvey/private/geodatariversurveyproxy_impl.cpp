#include "geodatariversurveyproxy_impl.h"

GeoDataRiverSurveyProxy::Impl::Impl() :
	m_centerAndBankLinesActor {vtkActor::New()},
	m_crossectionsActor {vtkActor::New()},
	m_backgroundActor {vtkActor::New()},
	m_crosssectionLinesActor {vtkActor::New()},
	m_labelMapper {vtkLabeledDataMapper::New()},
	m_labelActor {vtkActor2D::New()},
	m_crosssectionLines {vtkPolyData::New()}
{}

GeoDataRiverSurveyProxy::Impl::~Impl()
{
	m_centerAndBankLinesActor->Delete();
	m_crossectionsActor->Delete();
	m_backgroundActor->Delete();
	m_crosssectionLinesActor->Delete();
	m_labelMapper->Delete();
	m_labelActor->Delete();
	m_crosssectionLines->Delete();
}
