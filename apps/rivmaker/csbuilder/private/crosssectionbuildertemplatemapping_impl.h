#ifndef CROSSSECTIONBUILDERTEMPLATEMAPPING_IMPL_H
#define CROSSSECTIONBUILDERTEMPLATEMAPPING_IMPL_H

#include "../crosssectionbuildertemplatemapping.h"

class CrossSectionBuilderTemplateMapping::Impl
{
public:
	Impl();

	double m_mappingResolution;
	double m_streamWiseLength;
	double m_crossStreamWidth;
	int m_numberOfExpansions;
	double m_weightExponent;
};

#endif // CROSSSECTIONBUILDERTEMPLATEMAPPING_IMPL_H
