#ifndef CROSSSECTIONBUILDERTEMPLATEMAPPING_H
#define CROSSSECTIONBUILDERTEMPLATEMAPPING_H

#include "crosssectionbuilderi.h"

class CrossSectionBuilderTemplateMapping : public CrossSectionBuilderI
{
public:
	CrossSectionBuilderTemplateMapping();
	~CrossSectionBuilderTemplateMapping();

	double mappingResolution() const;
	void setMappingResolution(double resolution);

	double streamWiseLength() const;
	void setStreamWiseLength(double len);

	double crossStreamWidth() const;
	void setCrossStreamWidth(double width);

	int numberOfExpansions() const;
	void setNumberOfExpansions(int num);

	double weightExponent() const;
	void setWeightExponent(double exp);

	void build(const ElevationPoints& points, CrossSections* crossSections) override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/crosssectionbuildertemplatemapping_impl.h"
#endif // _DEBUG

#endif // CROSSSECTIONBUILDERTEMPLATEMAPPING_H
