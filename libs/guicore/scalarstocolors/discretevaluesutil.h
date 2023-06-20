#ifndef DISCRETEVALUESUTIL_H
#define DISCRETEVALUESUTIL_H

#include "../guicore_global.h"

#include "colormapsettingvaluecolorpaircontainer.h"

class vtkDataArray;
class vtkIntArray;
class vtkScalarsToColors;

class GUICOREDLL_EXPORT DiscreteValuesUtil
{
public:
	static vtkIntArray* buildValues(vtkDataArray* data, bool fillLower, bool fillUpper, double minValue, const std::vector<ColorMapSettingValueColorPairContainer>& colors, bool ignoreTransparent);
	static vtkIntArray* buildValuesEnumerate(vtkDataArray* data, const std::vector<ColorMapSettingValueColorPairContainer>& colors, bool ignoreTransparent);
	static vtkScalarsToColors* buildScalarsToColors(const std::vector<ColorMapSettingValueColorPairContainer>& colors, bool ignoreTransparent);

private:
	DiscreteValuesUtil();
};

#endif // DISCRETEVALUESUTIL_H
