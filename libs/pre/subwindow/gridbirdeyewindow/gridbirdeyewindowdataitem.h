#ifndef POST2DBIRDEYEWINDOWDATAITEM_H
#define POST2DBIRDEYEWINDOWDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <guicore/datamodel/graphicswindowdataitem.h>
#include "gridbirdeyewindowdatamodel.h"
#include <QList>

class GridBirdEyeWindow;
class GridBirdEyeWindowDataModel;

class GridBirdEyeWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowDataItem(GraphicsWindowDataItem* parent);
	GridBirdEyeWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	GridBirdEyeWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);

	GridBirdEyeWindow* mainWindow();

protected:
	GridBirdEyeWindowDataModel* dataModel() const;
};

#endif // POST2DBIRDEYEWINDOWDATAITEM_H
