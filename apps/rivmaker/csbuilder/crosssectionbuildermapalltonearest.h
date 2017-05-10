#ifndef CROSSSECTIONBUILDERMAPALLTONEAREST_H
#define CROSSSECTIONBUILDERMAPALLTONEAREST_H

#include "crosssectionbuilderi.h"

class CrossSectionBuilderMapAllToNearest : public CrossSectionBuilderI
{
public:
	CrossSectionBuilderMapAllToNearest();

	void build(const ElevationPoints& points, CrossSections* crossSections) override;
};

#endif // CROSSSECTIONBUILDERMAPALLTONEAREST_H
