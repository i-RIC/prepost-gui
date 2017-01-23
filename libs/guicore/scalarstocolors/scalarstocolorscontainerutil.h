#ifndef SCALARSTOCOLORSCONTAINERUTIL_H
#define SCALARSTOCOLORSCONTAINERUTIL_H

#include "../guicore_global.h"

#include <vector>

class ScalarsToColorsContainer;
class vtkDataArray;

class GUICOREDLL_EXPORT ScalarsToColorsContainerUtil
{
public:
	static void setValueRange(ScalarsToColorsContainer* c, const std::vector<vtkDataArray*> data);
	static void setValueRange(ScalarsToColorsContainer* c, vtkDataArray* data);

private:
	ScalarsToColorsContainerUtil();
};

#endif // SCALARSTOCOLORSCONTAINERUTIL_H
