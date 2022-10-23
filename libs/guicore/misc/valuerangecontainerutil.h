#ifndef VALUERANGECONTAINERUTIL_H
#define VALUERANGECONTAINERUTIL_H

#include "../guicore_global.h"

#include <vector>

class ValueRangeContainer;

class GUICOREDLL_EXPORT ValueRangeContainerUtil
{
public:
	static ValueRangeContainer build(const std::vector<vtkDataArray*> data);
	static ValueRangeContainer build(vtkDataArray* data);

private:
	ValueRangeContainerUtil();
};

#endif // VALUERANGECONTAINERUTIL_H
