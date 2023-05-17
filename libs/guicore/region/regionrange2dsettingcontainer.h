#ifndef REGIONRANGE2DSETTINGCONTAINER_H
#define REGIONRANGE2DSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>

class vtkStructuredGrid;

class GUICOREDLL_EXPORT RegionRange2dSettingContainer : public CompositeContainer
{
public:
	RegionRange2dSettingContainer();
	RegionRange2dSettingContainer(const RegionRange2dSettingContainer& s);

	RegionRange2dSettingContainer& operator=(const RegionRange2dSettingContainer& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s) override;

	vtkStructuredGrid* buildNodeFilteredData(vtkStructuredGrid* data) const;
	vtkStructuredGrid* buildCellFilteredData(vtkStructuredGrid* data) const;

	IntContainer iMin;
	IntContainer iMax;
	IntContainer jMin;
	IntContainer jMax;
};

#endif // REGIONRANGE2DSETTINGCONTAINER_H
