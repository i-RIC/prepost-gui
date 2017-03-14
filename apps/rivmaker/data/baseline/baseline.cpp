#include "baseline.h"

#include "private/baseline_impl.h"

#include <algorithm>

BaseLine::BaseLine(DataItem *parent) :
	DataItem {parent},
	impl {new Impl {}}
{}

BaseLine::~BaseLine()
{
	delete impl;
}

const std::vector<QPointF>& BaseLine::polyLine() const
{
	return impl->m_polyLine;
}

void BaseLine::setPolyLine(const std::vector<QPointF>& line)
{
	impl->m_polyLine = line;
}

bool BaseLine::checkIfCrosses(const CrossSection& cs) const
{
	// @todo implement this
	return true;
}

void BaseLine::getCrossingPoint(const CrossSection& cs, bool* crosses, double* x, double* y, double* pos)
{
	// @todo implement this
}

void BaseLine::normalizeDirection(CrossSection* cs) const
{
	// @todo implement this
}

void BaseLine::reverseDirection()
{
	std::reverse(impl->m_polyLine.begin(), impl->m_polyLine.end());
}
