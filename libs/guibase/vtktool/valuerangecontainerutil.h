#ifndef VALUERANGECONTAINERUTIL_H
#define VALUERANGECONTAINERUTIL_H

#include "../guibase_global.h"

#include <vector>

class ValueRangeContainer;

class GUIBASEDLL_EXPORT ValueRangeContainerUtil
{
public:
	static ValueRangeContainer build(const std::vector<vtkDataArray*> data);
	static ValueRangeContainer build(vtkDataArray* data);

private:
	ValueRangeContainerUtil();
};

#endif // VALUERANGECONTAINERUTIL_H
