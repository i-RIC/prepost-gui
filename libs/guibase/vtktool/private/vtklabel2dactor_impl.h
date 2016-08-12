#ifndef VTKLABEL2DACTOR_IMPL_H
#define VTKLABEL2DACTOR_IMPL_H

#include "../vtklabel2dactor.h"

#include <QPointF>

class vtkPoints;
class vtkPolyData;
class vtkLabeledDataMapper;
class vtkStringArray;

class vtkLabel2DActor::Impl
{
public:
	Impl();
	~Impl();

	void setup();

	vtkActor2D* m_actor;
	vtkPoints* m_points;
	vtkPolyData* m_polyData;
	vtkLabeledDataMapper* m_mapper;
	vtkStringArray* m_stringArray;

	QPointF m_position;
	LabelPosition m_labelPosition;
	std::string m_label;
};

#endif // VTKLABEL2DACTOR_IMPL_H
