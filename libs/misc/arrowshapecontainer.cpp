#include "arrowshapecontainer.h"

const int ArrowShapeContainer::DEFAULT_ARROWSIZE = 8;
const int ArrowShapeContainer::DEFAULT_LINEWIDTH = 1;

ArrowShapeContainer::ArrowShapeContainer() :
	CompositeContainer({&arrowSize, &lineWidth}),
	arrowSize {"arrowSize", DEFAULT_ARROWSIZE},
	lineWidth {"arrowLineWidth", DEFAULT_LINEWIDTH}
{}

ArrowShapeContainer::ArrowShapeContainer(const ArrowShapeContainer& c) :
	ArrowShapeContainer()
{
	copyValue(c);
}

ArrowShapeContainer& ArrowShapeContainer::operator=(const ArrowShapeContainer& c)
{
	copyValue(c);
	return *this;
}
