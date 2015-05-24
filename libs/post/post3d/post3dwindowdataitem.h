#ifndef POST3DWINDOWDATAITEM_H
#define POST3DWINDOWDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <guicore/datamodel/graphicswindowdataitem.h>
#include "post3dwindowdatamodel.h"
#include <QList>

class Post3dWindow;
class Post3dWindowDataModel;
class PostSolutionInfo;

class Post3dWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowDataItem(GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(parent)
	{}
	/// Constructor
	Post3dWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, parent)
	{}
	/// Constructor
	Post3dWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, icon, parent)
	{}
	Post3dWindow* postProcessorWindow();
	void updateZDepthRangeItemCount() {}
protected:
	void assignActorZValues(const ZDepthRange& /*range*/) {}
	Post3dWindowDataModel* dataModel();
};

#endif // POST3DWINDOWDATAITEM_H
