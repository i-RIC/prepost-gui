#ifndef POST3DWINDOWDATAITEM_H
#define POST3DWINDOWDATAITEM_H

#include "post3dwindow.h"
#include "post3dwindowdatamodel.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class Post3dWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowDataItem(GraphicsWindowDataItem* parent);
	Post3dWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	Post3dWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	~Post3dWindowDataItem();

	Post3dWindow* postProcessorWindow() const;
	void updateZDepthRangeItemCount() override;

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	Post3dWindowDataModel* dataModel() const;
};

#endif // POST3DWINDOWDATAITEM_H
