#include "post2dwindow.h"
#include "post2dwindowdataitem.h"
#include "post2dwindowdatamodel.h"
#include "post2dwindowgraphicsview.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

Post2dWindow* Post2dWindowDataItem::postProcessorWindow()
{
	return dynamic_cast<Post2dWindow*>(dataModel()->mainWindow());
}

Post2dWindowDataModel* Post2dWindowDataItem::dataModel()
{
	return dynamic_cast<Post2dWindowDataModel*>(GraphicsWindowDataItem::dataModel());
}
