#include "colormapsettingcontainer.h"
#include "colormapsettingcontainerutil.h"

#include <vtkDataArray.h>

void ColorMapSettingContainerUtil::setValueRange(ColorMapSettingContainer* c, const std::vector<vtkDataArray*> data)
{
	double min, max, range[2];
	min = 0; max = 0;
	bool first = true;
	for (auto d : data) {
		d->GetRange(range);
		if (first || range[0] < min) {min = range[0];}
		if (first || range[1] > max) {max = range[1];}
		first = false;
	}

	if (max - min < 1E-4) {
		// the width is too small.
		double mid = (min + max) * 0.5;
		double width = 1E-4;

		min = mid - width;
		max = mid + width;
	}
	c->setAutoValueRange(min, max);
}

void ColorMapSettingContainerUtil::setValueRange(ColorMapSettingContainer* c, vtkDataArray* data)
{
	std::vector<vtkDataArray*> v;
	if (data != nullptr) {
		v.push_back(data);
	}
	setValueRange(c, v);
}

ColorMapSettingContainerUtil::ColorMapSettingContainerUtil()
{}
