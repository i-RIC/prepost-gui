#ifndef STRUCTUREDGRIDREGION_H
#define STRUCTUREDGRIDREGION_H

/// Misc class to define some structs and enum to define structured grid regions
class StructuredGridRegion
{
public:
	/// Two-dimensional structured grid range
	struct Range2d {
		int iMin;
		int iMax;
		int jMin;
		int jMax;
	};

	/// Three-dimensional structured grid range
	struct Range3d {
		int iMin;
		int iMax;
		int jMin;
		int jMax;
		int kMin;
		int kMax;
	};

	/// Region selection mode on structured grid
	enum RegionMode {
		rmFull,    ///< Select full region
		rmActive,  ///< Select the active region (wet region)
		rmCustom   ///< Select custom region (select using Range2d or Range3d)
	};
};

#endif // STRUCTUREDGRIDREGION_H
