#ifndef NAMEDGRAPHICSWINDOWDATAITEMTOOL_H
#define NAMEDGRAPHICSWINDOWDATAITEMTOOL_H

#include "../guicore_global.h"

#include <QList>

class GraphicsWindowDataItem;

class GUICOREDLL_EXPORT NamedGraphicsWindowDataItemTool
{
public:
	static void checkItemWithName(const std::string& name, const QList<GraphicsWindowDataItem*>& items);

private:
	NamedGraphicsWindowDataItemTool();
	~NamedGraphicsWindowDataItemTool();
};

#endif // NAMEDGRAPHICSWINDOWDATAITEMTOOL_H
