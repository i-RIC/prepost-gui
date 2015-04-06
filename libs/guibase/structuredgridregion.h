#ifndef STRUCTUREDGRIDREGION_H
#define STRUCTUREDGRIDREGION_H

class StructuredGridRegion
{
public:
	struct Range2d {
		int iMin;
		int iMax;
		int jMin;
		int jMax;
	};

	struct Range3d {
		int iMin;
		int iMax;
		int jMin;
		int jMax;
		int kMin;
		int kMax;
	};

	enum RegionMode {
		rmFull,
		rmActive,
		rmCustom
	};
};

#endif // STRUCTUREDGRIDREGION_H
