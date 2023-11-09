#include "valuerangecontainerutil.h"

#include <misc/valuerangecontainer.h>

#include <vtkDataArray.h>

namespace {

const double MIN_WIDTH = 2.0E-4;

} // namespace

ValueRangeContainer ValueRangeContainerUtil::build(const std::vector<vtkDataArray*> data)
{
	ValueRangeContainer ret;
	double range[2];

	bool first = true;
	for (auto d : data) {
		d->GetRange(range);
		if (first || range[0] < ret.minValue) {ret.minValue = range[0];}
		if (first || range[1] > ret.maxValue) {ret.maxValue = range[1];}
		first = false;
	}

	if (ret.maxValue - ret.minValue < MIN_WIDTH) {
		double mid = (ret.minValue + ret.maxValue) * 0.5;
		ret.minValue = mid - MIN_WIDTH * 0.5;
		ret.maxValue = mid + MIN_WIDTH * 0.5;
	}

	return ret;
}

ValueRangeContainer ValueRangeContainerUtil::build(vtkDataArray* data)
{
	std::vector<vtkDataArray*> v;
	if (data != nullptr) {
		v.push_back(data);
	}

	return build(v);
}
