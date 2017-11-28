#include "attributebrowserhelper.h"
#include "preprocessorgridtypedataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>

bool AttributeBrowserHelper::isAttributeBrowserAvailable(PreProcessorGridTypeDataItem* ditem)
{
	SolverDefinitionGridType::GridType gt = ditem->gridType()->defaultGridType();
	return (gt == SolverDefinitionGridType::gtStructured2DGrid ||
					gt == SolverDefinitionGridType::gtUnstructured2DGrid);
}

AttributeBrowserHelper::AttributeBrowserHelper()
{}
