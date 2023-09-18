#ifndef V4STRUCTURED15DGRIDWITHCROSSSECTION_H
#define V4STRUCTURED15DGRIDWITHCROSSSECTION_H

#include "../guicore_global.h"

#include "v4grid2dt.h"

#include <guibase/vtkpointsetextended/vtkstructuredgridextended2d.h>

class v4Structured15dGridWithCrossSectionCrossSection;

class vtkPolyData;

class GUICOREDLL_EXPORT v4Structured15dGridWithCrossSection : public v4Grid2dT<vtkStructuredGridExtended2d>
{
public:
	v4Structured15dGridWithCrossSection();
	~v4Structured15dGridWithCrossSection();

	void setCrossSectionCount(unsigned int count);

	vtkPolyData* pointsGrid() const;

	const std::vector<v4Structured15dGridWithCrossSectionCrossSection*>& crossSections() const;
	std::vector<v4Structured15dGridWithCrossSectionCrossSection*>& crossSections();
	v4Structured15dGridWithCrossSectionCrossSection* crossSection(int index);

	void updateFilteredData(double xMin, double xMax, double yMin, double yMax) override;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // V4STRUCTURED15DGRIDWITHCROSSSECTION_H
