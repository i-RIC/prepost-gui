#include "post2dbirdeyewindowdataitem.h"
#include <guicore/project/projectdata.h>
#include "post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowdatamodel.h"
#include "post2dbirdeyewindow.h"
#include <guicore/project/projectmainfile.h>

Post2dBirdEyeWindow* Post2dBirdEyeWindowDataItem::postProcessorWindow()
{
	return dynamic_cast<Post2dBirdEyeWindow*>(dataModel()->mainWindow());
}

Post2dBirdEyeWindowDataModel* Post2dBirdEyeWindowDataItem::dataModel()
{
	return dynamic_cast<Post2dBirdEyeWindowDataModel*>(GraphicsWindowDataItem::dataModel());
}
