#ifndef POSTPOLYDATABASICSETTING_H
#define POSTPOLYDATABASICSETTING_H

#include "../postbase_global.h"

#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/intcontainer.h>

class POSTBASEDLL_EXPORT PostPolyDataBasicSetting : public CompositeContainer
{
public:
	PostPolyDataBasicSetting();
	PostPolyDataBasicSetting(const PostPolyDataBasicSetting& s);

	PostPolyDataBasicSetting& operator=(const PostPolyDataBasicSetting& c);

	ColorContainer color;
	IntContainer lineWidth;
};

#endif // POSTPOLYDATABASICSETTING_H
