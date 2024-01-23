#ifndef NAMEDQSTRINGGRAPHICSWINDOWDATAITEMTOOL_H
#define NAMEDQSTRINGGRAPHICSWINDOWDATAITEMTOOL_H

#include "../guicore_global.h"

#include <vector>
#include <string>

class GraphicsWindowDataItem;

class GUICOREDLL_EXPORT NamedQStringGraphicsWindowDataItemTool
{
public:
	static bool checkItemWithName(const QString& name, const std::vector<GraphicsWindowDataItem*>& items, bool noSignal = false);

private:
	NamedQStringGraphicsWindowDataItemTool();
	~NamedQStringGraphicsWindowDataItemTool();
};

#endif // NAMEDQSTRINGGRAPHICSWINDOWDATAITEMTOOL_H
