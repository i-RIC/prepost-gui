#ifndef STRUCTUREDGRIDREGION_H
#define STRUCTUREDGRIDREGION_H

#include "guibase_global.h"

#include <misc/intcontainer.h>
#include <misc/compositecontainer.h>

/// Misc class to define some structs and enum to define structured grid regions
class StructuredGridRegion
{

public:
	/// Two-dimensional structured grid range
	struct GUIBASEDLL_EXPORT Range2d : public CompositeContainer
	{
		/// Constructor
		Range2d();
		/// Copy constructor
		Range2d(const Range2d& r);
		/// Copy operator
		Range2d& operator=(const Range2d& r);

		IntContainer iMin;
		IntContainer iMax;
		IntContainer jMin;
		IntContainer jMax;
	};

	/// Three-dimensional structured grid range
	struct GUIBASEDLL_EXPORT Range3d : public CompositeContainer
	{
		/// Constructor
		Range3d();
		/// Copy constructor
		Range3d(const Range3d& r);
		/// Copy operator
		Range3d& operator=(const Range3d& r);

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
