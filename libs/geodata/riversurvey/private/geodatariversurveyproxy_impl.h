#ifndef GEODATARIVERSURVEYPROXY_IMPL_H
#define GEODATARIVERSURVEYPROXY_IMPL_H

#include "../geodatariversurveyproxy.h"
#include "geodatariversurveyproxy_displaysetting.h"

class GeoDataRiverSurveyProxy::Impl
{
public:
	Impl();
	~Impl();

	DisplaySetting m_displaySetting;

	vtkActor* m_centerAndBankLinesActor;
	vtkActor* m_crossectionsActor;

	vtkActor* m_backgroundActor;
	vtkActor* m_crosssectionLinesActor;

	vtkLabeledDataMapper* m_labelMapper;
	vtkActor2D* m_labelActor;

	vtkPolyData* m_crosssectionLines;
};

#endif // GEODATARIVERSURVEYPROXY_IMPL_H
