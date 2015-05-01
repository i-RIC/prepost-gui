#ifndef POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H
#define POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H

#include "../post2d_global.h"
#include "../post2dwindowdataitem.h"

class SolverDefinitionGridType;
class LookupTableContainer;

class POST2D_EXPORT Post2dWindowGridTypeDataItemInterface
{
public:
	virtual ~Post2dWindowGridTypeDataItemInterface() {}
	virtual SolverDefinitionGridType* gridType() = 0;
	virtual LookupTableContainer* lookupTable(const QString& attName) = 0;

};

#endif // POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H
