#include "logodataitem_impl.h"

#include <vtkActor2D.h>

LogoDataItem::Impl::Impl() :
	m_actor {vtkActor2D::New()}
{}

LogoDataItem::Impl::~Impl()
{
	m_actor->Delete();
}
