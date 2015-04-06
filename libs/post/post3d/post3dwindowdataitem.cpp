#include "post3dwindow.h"
#include "post3dwindowdataitem.h"
#include "post3dwindowdatamodel.h"
#include "post3dwindowgraphicsview.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

Post3dWindow* Post3dWindowDataItem::postProcessorWindow()
{
	return dynamic_cast<Post3dWindow*>(dataModel()->mainWindow());
}

Post3dWindowDataModel* Post3dWindowDataItem::dataModel()
{
	return dynamic_cast<Post3dWindowDataModel*>(GraphicsWindowDataItem::dataModel());
}
