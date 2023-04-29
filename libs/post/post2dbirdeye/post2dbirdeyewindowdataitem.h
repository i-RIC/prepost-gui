#ifndef POST2DBIRDEYEWINDOWDATAITEM_H
#define POST2DBIRDEYEWINDOWDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <guicore/datamodel/graphicswindowdataitem.h>
#include "post2dbirdeyewindowdatamodel.h"
#include <QList>

class Post2dBirdEyeWindow;
class Post2dBirdEyeWindowDataModel;
class PostSolutionInfo;

class Post2dBirdEyeWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowDataItem(GraphicsWindowDataItem* parent);
	Post2dBirdEyeWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	Post2dBirdEyeWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);

	Post2dBirdEyeWindow* postProcessorWindow();

protected:
	Post2dBirdEyeWindowDataModel* dataModel();
};

#endif // POST2DBIRDEYEWINDOWDATAITEM_H
