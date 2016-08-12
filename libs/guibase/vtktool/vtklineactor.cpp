#include "vtklineactor.h"
#include "private/vtklineactor_impl.h"

vtkLineActor::vtkLineActor() :
	impl {new Impl()}
{}

vtkLineActor::~vtkLineActor()
{
	delete impl;
}

vtkActor* vtkLineActor::pointsActor() const
{
	return impl->m_actor.pointsActor();
}

vtkActor* vtkLineActor::lineActor() const
{
	return impl->m_actor.linesActor();
}

void vtkLineActor::setLine(const std::vector<QPointF>& line)
{
	impl->m_line = line;

	std::vector<std::vector<QPointF> > lines;
	lines.push_back(line);
	impl->m_actor.setLines(lines);
}

std::vector<QPointF> vtkLineActor::line() const
{
	return impl->m_line;
}
