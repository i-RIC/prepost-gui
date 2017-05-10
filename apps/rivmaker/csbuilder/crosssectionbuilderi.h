#ifndef CROSSSECTIONBUILDERI_H
#define CROSSSECTIONBUILDERI_H

class ElevationPoints;
class CrossSections;

class CrossSectionBuilderI
{
public:
	virtual ~CrossSectionBuilderI() {}

	virtual void build(const ElevationPoints& points, CrossSections* crossSections) = 0;
};

#endif // CROSSSECTIONBUILDERI_H
