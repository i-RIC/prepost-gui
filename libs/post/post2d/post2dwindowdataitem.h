#ifndef POST2DWINDOWDATAITEM_H
#define POST2DWINDOWDATAITEM_H

#include "post2d_global.h"
#include "post2dwindow.h"
#include "post2dwindowdatamodel.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class POST2D_EXPORT Post2dWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowDataItem(GraphicsWindowDataItem* parent);
	Post2dWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	Post2dWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	virtual ~Post2dWindowDataItem();

	Post2dWindow* postProcessorWindow() const;

protected:
	Post2dWindowDataModel* dataModel() const;
};

#endif // POST2DWINDOWDATAITEM_H
