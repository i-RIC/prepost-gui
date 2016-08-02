#ifndef POST2DWINDOWGRAPHSETTING_H
#define POST2DWINDOWGRAPHSETTING_H

#include <guibase/vtklinestylecontainer.h>

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/intvectorcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/stringcontainer.h>

class Post2dWindowGraphSetting : public CompositeContainer
{
public:
	enum Direction {
		dirI,
		dirJ
	};

	enum Region {
		All,
		Partial,
		Custom
	};

	enum OffsetDirection {
		Axis,
		Vertical
	};

	enum ValueFix {
		NoFix,
		SubtractMax,
		SubtractMin,
	};

	Post2dWindowGraphSetting();
	Post2dWindowGraphSetting(const Post2dWindowGraphSetting& setting);
	~Post2dWindowGraphSetting();

	Post2dWindowGraphSetting& operator=(const Post2dWindowGraphSetting& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<Direction> direction;

	EnumContainerT<Region> region;

	IntContainer regionStartIndex;
	IntContainer regionEndIndex;
	IntContainer regionSkipIndex;

	IntVectorContainer regionIndices;

	EnumContainerT<OffsetDirection> offsetDirection;

	BoolContainer drawGridLine;
	vtkLineStyleContainer gridLineStyle;

	StringContainer graphTarget;
	DoubleContainer graphScale;
	vtkLineStyleContainer graphLineStyle;
	EnumContainerT<ValueFix> graphValueFix;
};

#endif // POST2DWINDOWGRAPHSETTING_H
