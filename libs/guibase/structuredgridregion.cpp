#include "structuredgridregion.h"

StructuredGridRegion::Range2d::Range2d() :
	CompositeContainer ({&iMin, &iMax, &jMin, &jMax}),
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"}
{}

StructuredGridRegion::Range2d::Range2d(const Range2d& r) :
	Range2d()
{
	CompositeContainer::copyValue(r);
}

StructuredGridRegion::Range2d& StructuredGridRegion::Range2d::operator=(const Range2d& r)
{
	CompositeContainer::copyValue(r);
	return *this;
}

XmlAttributeContainer& StructuredGridRegion::Range2d::operator=(const XmlAttributeContainer& r)
{
	return operator=(dynamic_cast<const StructuredGridRegion::Range2d&> (r));
}

StructuredGridRegion::Range3d::Range3d() :
	CompositeContainer ({&iMin, &iMax, &jMin, &jMax, &kMin, &kMax}),
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"},
	kMin {"kMin"},
	kMax {"kMax"}
{}

StructuredGridRegion::Range3d::Range3d(const Range3d& r) :
	Range3d()
{
	CompositeContainer::copyValue(r);
}

StructuredGridRegion::Range3d& StructuredGridRegion::Range3d::operator=(const Range3d& r)
{
	CompositeContainer::copyValue(r);
	return *this;
}

XmlAttributeContainer& StructuredGridRegion::Range3d::operator=(const XmlAttributeContainer& r)
{
	return operator=(dynamic_cast<const StructuredGridRegion::Range3d&> (r));
}
