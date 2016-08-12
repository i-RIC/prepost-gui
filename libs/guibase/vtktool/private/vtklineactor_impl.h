#ifndef VTKLINEACTOR_IMPL_H
#define VTKLINEACTOR_IMPL_H

#include "../vtklineactor.h"
#include "../vtklinesactor.h"

class vtkLineActor::Impl
{
public:
	vtkLinesActor m_actor;
	std::vector<QPointF> m_line;
};

#endif // VTKLINEACTOR_IMPL_H
