#include "postbaseiterativevaluescontainer_impl.h"
#include "postbaseiterativevaluescontainer_io.h"

PostBaseIterativeValuesContainer::Impl::Impl() :
	m_io {nullptr}
{}

PostBaseIterativeValuesContainer::Impl::~Impl()
{
	delete m_io;
}
