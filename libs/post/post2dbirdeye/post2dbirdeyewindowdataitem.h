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
	/// Constructor
	Post2dBirdEyeWindowDataItem(GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(parent)
	{}
	/// Constructor
	Post2dBirdEyeWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, parent)
	{}
	/// Constructor
	Post2dBirdEyeWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, icon, parent)
	{}
	Post2dBirdEyeWindow* postProcessorWindow();

protected:
	Post2dBirdEyeWindowDataModel* dataModel();
};

#endif // POST2DBIRDEYEWINDOWDATAITEM_H
