#ifndef VTKARROWSACTOR_IMPL_H
#define VTKARROWSACTOR_IMPL_H

#include "../vtkarrowsactor.h"

class vtkActor;
class vtkAppendPolyData;
class vtkConeSource;
class vtkGlyph3D;
class vtkHedgeHog;
class vtkPolyDataMapper;
class vtkWarpVector;

class vtkArrowsActor::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_actor;
	vtkPolyDataMapper* m_mapper;

	vtkAppendPolyData* m_conesAndHedgeHog;
	vtkConeSource* m_coneSource;
	vtkGlyph3D* m_glyph;
	vtkHedgeHog* m_hedgeHog;
	vtkWarpVector* m_warpVector;

	double m_angle;
};


#endif // VTKARROWSACTOR_IMPL_H
