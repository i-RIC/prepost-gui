#include "crosssectionbuildertemplatemapping.h"

#include "private/crosssectionbuildertemplatemapping_impl.h"

CrossSectionBuilderTemplateMapping::Impl::Impl() :
	m_mappingResolution {5},
	m_streamWiseLength {10},
	m_crossStreamWidth {5},
	m_numberOfExpansions {3},
	m_weightExponent {3}
{}

CrossSectionBuilderTemplateMapping::CrossSectionBuilderTemplateMapping() :
	impl {new Impl {}}
{}

CrossSectionBuilderTemplateMapping::~CrossSectionBuilderTemplateMapping()
{
	delete impl;
}

double CrossSectionBuilderTemplateMapping::mappingResolution() const
{
	return impl->m_mappingResolution;
}

void CrossSectionBuilderTemplateMapping::setMappingResolution(double resolution)
{
	impl->m_mappingResolution = resolution;
}

double CrossSectionBuilderTemplateMapping::streamWiseLength() const
{
	return impl->m_streamWiseLength;
}

void CrossSectionBuilderTemplateMapping::setStreamWiseLength(double len)
{
	impl->m_streamWiseLength = len;
}

double CrossSectionBuilderTemplateMapping::crossStreamWidth() const
{
	return impl->m_crossStreamWidth;
}

void CrossSectionBuilderTemplateMapping::setCrossStreamWidth(double width)
{
	impl->m_crossStreamWidth = width;
}

int CrossSectionBuilderTemplateMapping::numberOfExpansions() const
{
	return impl->m_numberOfExpansions;
}

void CrossSectionBuilderTemplateMapping::setNumberOfExpansions(int num)
{
	impl->m_numberOfExpansions = num;
}

double CrossSectionBuilderTemplateMapping::weightExponent() const
{
	return impl->m_weightExponent;
}

void CrossSectionBuilderTemplateMapping::setWeightExponent(double exp)
{
	impl->m_weightExponent = exp;
}

void CrossSectionBuilderTemplateMapping::build(const ElevationPoints& points, CrossSections* crossSections)
{

}
