#include "zdepthrange.h"
#include "private/zdepthrange_impl.h"

ZDepthRange::ZDepthRange() :
	m_impl {new Impl {}}
{}

ZDepthRange::ZDepthRange(double min, double max) :
	ZDepthRange()
{
	m_impl->m_min = min;
	m_impl->m_max = max;
}

ZDepthRange::ZDepthRange(const ZDepthRange& range) :
	ZDepthRange()
{
	*(m_impl) = *(range.m_impl);
}

ZDepthRange::ZDepthRange(ZDepthRange&& range) :
	m_impl {range.m_impl}
{
	range.m_impl = nullptr;
}

ZDepthRange::~ZDepthRange()
{
	delete m_impl;
}

ZDepthRange& ZDepthRange::operator=(const ZDepthRange& range)
{
	*(m_impl) = *(range.m_impl);
	return *this;
}

ZDepthRange& ZDepthRange::operator=(ZDepthRange&& range)
{
	delete m_impl;
	m_impl = range.m_impl;
	range.m_impl = nullptr;
	return *this;
}

double ZDepthRange::min() const
{
	return m_impl->m_min;
}

double ZDepthRange::max() const
{
	return m_impl->m_max;
}

void ZDepthRange::setMin(double min)
{
	m_impl->m_min = min;
}

void ZDepthRange::setMax(double max)
{
	m_impl->m_max = max;
}

void ZDepthRange::setRange(double min, double max)
{
	m_impl->m_min = min;
	m_impl->m_max = max;
}

double ZDepthRange::width() const
{
	return m_impl->m_max - m_impl->m_min;
}

unsigned int ZDepthRange::itemCount() const
{
	return m_impl->m_itemCount;
}

void ZDepthRange::setItemCount(unsigned int count)
{
	m_impl->m_itemCount = count;
}
