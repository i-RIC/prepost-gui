#include "structuredgridregion.h"


StructuredGridRegion::Range2d::Range2d() :
	CompositeContainer {&iMin, &iMax, &jMin, &jMax},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"}
{}

StructuredGridRegion::Range3d::Range3d() :
	CompositeContainer {&iMin, &iMax, &jMin, &jMax, &kMin, &kMax},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"},
	kMin {"kMin"},
	kMax {"kMax"}
{}
