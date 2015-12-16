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
		Range2d();
		Range2d(const Range2d& r);

		Range2d& operator=(const Range2d& r);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& r) override;

		IntContainer iMin;
		IntContainer iMax;
		IntContainer jMin;
		IntContainer jMax;
	};

	/// Three-dimensional structured grid range
	struct GUIBASEDLL_EXPORT Range3d : public CompositeContainer
	{
		Range3d();
		Range3d(const Range3d& r);

		Range3d& operator=(const Range3d& r);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& r) override;

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
