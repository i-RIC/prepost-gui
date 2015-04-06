#ifndef POST2DWINDOWDATAITEM_H
#define POST2DWINDOWDATAITEM_H

#include "post2d_global.h"
#include <guicore/project/projectdataitem.h>
#include <guicore/datamodel/graphicswindowdataitem.h>
#include "post2dwindowdatamodel.h"
#include <QList>

class Post2dWindow;
class Post2dWindowDataModel;
class PostSolutionInfo;

class POST2D_EXPORT Post2dWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowDataItem(GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(parent)
	{}
	/// Constructor
	Post2dWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, parent)
	{}
	/// Constructor
	Post2dWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, icon, parent)
	{}
	Post2dWindow* postProcessorWindow();

protected:
	Post2dWindowDataModel* dataModel();
};

#endif // POST2DWINDOWDATAITEM_H
