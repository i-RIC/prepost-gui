#include "private/vtkactorpolydatamapperpair_impl.h"
#include "vtkactorpolydatamapperpair.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

vtkActorPolyDataMapperPair::Impl::Impl() :
	m_actor {vtkActor::New()},
	m_mapper {vtkPolyDataMapper::New()}
{
	m_actor->SetMapper(m_mapper);

	auto p = m_actor->GetProperty();
	p->SetLighting(false);

	m_mapper->SetScalarVisibility(0);
}

vtkActorPolyDataMapperPair::Impl::~Impl()
{
	m_actor->Delete();
	m_mapper->Delete();
}

vtkActorPolyDataMapperPair::vtkActorPolyDataMapperPair() :
	impl {new Impl()}
{}

vtkActorPolyDataMapperPair::~vtkActorPolyDataMapperPair()
{
	delete impl;
}

vtkActor* vtkActorPolyDataMapperPair::actor() const
{
	return impl->m_actor;
}

vtkPolyDataMapper* vtkActorPolyDataMapperPair::mapper() const
{
	return impl->m_mapper;
}
