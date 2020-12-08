#ifndef NAMEDGRAPHICSWINDOWDATAITEMTOOL_H
#define NAMEDGRAPHICSWINDOWDATAITEMTOOL_H

#include "../guicore_global.h"

#include <vector>
#include <string>

class GraphicsWindowDataItem;

class GUICOREDLL_EXPORT NamedGraphicsWindowDataItemTool
{
public:
	static bool checkItemWithName(const std::string& name, const std::vector<GraphicsWindowDataItem*>& items, bool noSignal = false);

private:
	NamedGraphicsWindowDataItemTool();
	~NamedGraphicsWindowDataItemTool();
};

#endif // NAMEDGRAPHICSWINDOWDATAITEMTOOL_H
