#include "gridcreatingconditioncreatorrectangularregionlonlat.h"
#include "gridcreatingconditionrectangularregionlonlat.h"

GridCreatingConditionCreatorRectangularRegionLonLat::GridCreatingConditionCreatorRectangularRegionLonLat()
	: GridCreatingConditionCreator()
{
	m_name = "rectangular_region_lonlat";
	m_caption = tr("Create grid by dividing rectangular region (Longitude-Latitude)");
	m_description = tr(
		"Specify a rectangular region where to create a grid, "
		"and the cell width. The cell width is used for both "
		"X-direction and Y-direction. The coordinate system is"
		"Longitude-Latitude system."
			);
}

GridCreatingCondition* GridCreatingConditionCreatorRectangularRegionLonLat::create(ProjectDataItem *parent)
{
	return new GridCreatingConditionRectangularRegionLonLat(parent, this);
}
