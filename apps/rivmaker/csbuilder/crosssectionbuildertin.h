#ifndef CROSSSECTIONBUILDERTIN_H
#define CROSSSECTIONBUILDERTIN_H

#include "crosssectionbuilderi.h"

class CrossSectionBuilderTin : public CrossSectionBuilderI
{
public:
	CrossSectionBuilderTin();

	void build(const ElevationPoints& points, CrossSections* crossSections) override;
};

#endif // CROSSSECTIONBUILDERTIN_H
