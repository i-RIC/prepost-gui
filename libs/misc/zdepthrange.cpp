#include "zdepthrange.h"
#include "private/zdepthrange_impl.h"

ZDepthRange::ZDepthRange() :
	impl {new Impl {}}
{}

ZDepthRange::ZDepthRange(double min, double max) :
	ZDepthRange()
{
	impl->m_min = min;
	impl->m_max = max;
}

ZDepthRange::ZDepthRange(const ZDepthRange& range) :
	ZDepthRange()
{
	*(impl) = *(range.impl);
}

ZDepthRange::ZDepthRange(ZDepthRange&& range) :
	impl {range.impl}
{
	range.impl = nullptr;
}

ZDepthRange::~ZDepthRange()
{
	delete impl;
}

ZDepthRange& ZDepthRange::operator=(const ZDepthRange& range)
{
	*(impl) = *(range.impl);
	return *this;
}

ZDepthRange& ZDepthRange::operator=(ZDepthRange&& range)
{
	delete impl;
	impl = range.impl;
	range.impl = nullptr;
	return *this;
}

double ZDepthRange::min() const
{
	return impl->m_min;
}

double ZDepthRange::max() const
{
	return impl->m_max;
}

void ZDepthRange::setMin(double min)
{
	impl->m_min = min;
}

void ZDepthRange::setMax(double max)
{
	impl->m_max = max;
}

void ZDepthRange::setRange(double min, double max)
{
	impl->m_min = min;
	impl->m_max = max;
}

double ZDepthRange::width() const
{
	return impl->m_max - impl->m_min;
}

unsigned int ZDepthRange::itemCount() const
{
	return impl->m_itemCount;
}

void ZDepthRange::setItemCount(unsigned int count)
{
	impl->m_itemCount = count;
}
