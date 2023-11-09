#ifndef V4STRUCTURED15DGRIDWITHCROSSSECTION_IMPL_H
#define V4STRUCTURED15DGRIDWITHCROSSSECTION_IMPL_H

#include "../v4structured15dgridwithcrosssection.h"
#include "../v4structured15dgridwithcrosssectioncrosssection.h"

class v4Structured15dGridWithCrossSection::Impl
{
public:
	Impl(v4Structured15dGridWithCrossSection* parent);
	~Impl();

	void setupVtkObjects(int crossSectionCount);

	vtkPolyData* m_pointsGrid;
	std::vector<v4Structured15dGridWithCrossSectionCrossSection*> m_crossSections;

private:
	v4Structured15dGridWithCrossSection* m_parent;
};

#endif // V4STRUCTURED15DGRIDWITHCROSSSECTION_IMPL_H
