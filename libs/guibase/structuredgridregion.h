#ifndef STRUCTUREDGRIDREGION_H
#define STRUCTUREDGRIDREGION_H

#include <misc/intcontainer.h>
#include <misc/compositecontainer.h>

/// Misc class to define some structs and enum to define structured grid regions
class StructuredGridRegion
{

public:
	/// Two-dimensional structured grid range
	struct Range2d : public CompositeContainer
	{
		Range2d();

		IntContainer iMin;
		IntContainer iMax;
		IntContainer jMin;
		IntContainer jMax;
	};

	/// Three-dimensional structured grid range
	struct Range3d : public CompositeContainer
	{
		Range3d();

		IntContainer iMin;
		IntContainer iMax;
		IntContainer jMin;
		IntContainer jMax;
		IntContainer kMin;
		IntContainer kMax;
	};

	/// Region selection mode on structured grid
	enum RegionMode {
		rmFull,    ///< Select full region
		rmActive,  ///< Select the active region (wet region)
		rmCustom   ///< Select custom region (select using Range2d or Range3d)
	};
};

#endif // STRUCTUREDGRIDREGION_H
