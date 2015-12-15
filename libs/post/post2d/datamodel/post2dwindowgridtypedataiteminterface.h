#ifndef POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H
#define POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H

#include "../post2d_global.h"
#include "../post2dwindowdataitem.h"

#include <string>

class SolverDefinitionGridType;
class LookupTableContainer;

class POST2D_EXPORT Post2dWindowGridTypeDataItemInterface
{
public:
	virtual ~Post2dWindowGridTypeDataItemInterface() {}
	virtual SolverDefinitionGridType* gridType() const = 0;
	virtual LookupTableContainer* nodeLookupTable(const std::string& attName) const = 0;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H
