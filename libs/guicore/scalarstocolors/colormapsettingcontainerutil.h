#ifndef COLORMAPSETTINGCONTAINERUTIL_H
#define COLORMAPSETTINGCONTAINERUTIL_H

#include "../guicore_global.h"

#include <vector>

class ColorMapSettingContainer;
class vtkDataArray;

class GUICOREDLL_EXPORT ColorMapSettingContainerUtil
{
public:
	static void setValueRange(ColorMapSettingContainer* c, const std::vector<vtkDataArray*> data);
	static void setValueRange(ColorMapSettingContainer* c, vtkDataArray* data);

private:
	ColorMapSettingContainerUtil();
};

#endif // COLORMAPSETTINGCONTAINERUTIL_H
