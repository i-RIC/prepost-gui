#ifndef ARROWSHAPECONTAINER_H
#define ARROWSHAPECONTAINER_H

#include "misc_global.h"
#include "compositecontainer.h"
#include "intcontainer.h"

/// Container for arrow shape settings
class MISCDLL_EXPORT ArrowShapeContainer : public XmlAttributeContainer
{
	static const int DEFAULT_ARROWSIZE;
	static const int DEFAULT_LINEWIDTH;

public:
	ArrowShapeContainer();
	ArrowShapeContainer(const ArrowShapeContainer& c);

	ArrowShapeContainer& operator=(const ArrowShapeContainer& c);

	IntContainer arrowSize;
	IntContainer lineWidth;
};

#endif // ARROWSHAPECONTAINER_H
