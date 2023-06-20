#include "discretevaluesutil.h"

#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>

#include <algorithm>
#include <unordered_map>

vtkIntArray* DiscreteValuesUtil::buildValues(vtkDataArray* data, bool fillLower, bool fillUpper, double minValue, const std::vector<ColorMapSettingValueColorPairContainer>& colors, bool ignoreTransparent)
{
	auto inputInt = vtkIntArray::SafeDownCast(data);
	auto inputDouble = vtkDoubleArray::SafeDownCast(data);

	std::vector<double> thresholds;
	std::vector<int> values;

	int value = 1;

	auto first = *colors.begin();
	thresholds.push_back(minValue);
	if ((first.transparent && ! ignoreTransparent) || ! fillLower) {
		values.push_back(0);
	} else {
		values.push_back(value);
	}

	for (const auto& pair : colors) {
		thresholds.push_back(pair.value.value());
		if (pair.transparent && ! ignoreTransparent) {
			values.push_back(0);
		} else {
			values.push_back(value);
			value += 1;
		}
	}

	int defaultVal = value;
	auto last = *colors.rbegin();
	if ((last.transparent && ! ignoreTransparent) || ! fillUpper) {
		defaultVal = 0;
	}

	auto ret = vtkIntArray::New();
	ret->SetName("MappedValue");
	ret->Allocate(data->GetNumberOfValues());

	if (inputInt != nullptr) {
		for (int i = 0; i < inputInt->GetNumberOfValues(); ++i) {
			double val = inputInt->GetValue(i);
			auto it = std::lower_bound(thresholds.begin(), thresholds.end(), val);
			int intVal = defaultVal;
			if (it != thresholds.end()) {
				int idx = it - thresholds.begin();
				intVal = values.at(idx);
			}
			ret->InsertNextValue(intVal);
		}
	} else if (inputDouble != nullptr) {
		for (int i = 0; i < inputDouble->GetNumberOfValues(); ++i) {
			double val = inputDouble->GetValue(i);
			auto it = std::lower_bound(thresholds.begin(), thresholds.end(), val);
			int intVal = defaultVal;
			if (it != thresholds.end()) {
				int idx = it - thresholds.begin();
				intVal = values.at(idx);
			}
			ret->InsertNextValue(intVal);
		}
	}
	return ret;
}

vtkIntArray* DiscreteValuesUtil::buildValuesEnumerate(vtkDataArray* data, const std::vector<ColorMapSettingValueColorPairContainer>& colors, bool ignoreTransparent)
{
	auto inputInt = vtkIntArray::SafeDownCast(data);
	auto inputDouble = vtkDoubleArray::SafeDownCast(data);

	std::unordered_map<double, int> valueMap;
	int value = 1;
	for (const auto& pair : colors) {
		if (pair.transparent && ! ignoreTransparent) {
			continue;
		}
		valueMap.insert({pair.value.value(), value});
		value += 1;
	}

	auto ret = vtkIntArray::New();
	ret->SetName("MappedValue");
	ret->Allocate(data->GetNumberOfValues());

	if (inputInt != nullptr) {
		for (int i = 0; i < inputInt->GetNumberOfValues(); ++i) {
			double val = inputInt->GetValue(i);
			auto it = valueMap.find(val);
			int intVal = 0;
			if (it != valueMap.end()) {
				intVal = it->second;
			}
			ret->InsertNextValue(intVal);
		}
	} else if (inputDouble != nullptr) {
		for (int i = 0; i < inputDouble->GetNumberOfValues(); ++i) {
			double val = inputDouble->GetValue(i);
			auto it = valueMap.find(val);
			int intVal = 0;
			if (it != valueMap.end()) {
				intVal = it->second;
			}
			ret->InsertNextValue(intVal);
		}
	}
	return ret;
}

vtkScalarsToColors* DiscreteValuesUtil::buildScalarsToColors(const std::vector<ColorMapSettingValueColorPairContainer>& colors, bool ignoreTransparent)
{
	auto ctf = vtkColorTransferFunction::New();
	int value = 1;

	for (const auto& pair : colors) {
		if (pair.transparent && ! ignoreTransparent) {
			continue;
		}

		QColor color = pair.color;
		ctf->AddRGBPoint(value, color.redF(), color.greenF(), color.blueF());
		value += 1;
	}

	return ctf;
}

DiscreteValuesUtil::DiscreteValuesUtil()
{}
