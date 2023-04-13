#ifndef POST2DWINDOWGRIDTYPEDATAITEM_PRECOLORMAPSETTINGUPDATEHANDLER_H
#define POST2DWINDOWGRIDTYPEDATAITEM_PRECOLORMAPSETTINGUPDATEHANDLER_H

#include "../post2dwindowgridtypedataitem.h"

#include <QObject>

class DelegatedColorMapSettingContainer;

class Post2dWindowGridTypeDataItem::PreColorMapSettingUpdateHandler : public QObject
{
	Q_OBJECT
public:
	PreColorMapSettingUpdateHandler(DelegatedColorMapSettingContainer* setting, VTKGraphicsView* view, QObject *parent);

public slots:
	void handle();

private:
	DelegatedColorMapSettingContainer* m_setting;
	VTKGraphicsView* m_view;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_PRECOLORMAPSETTINGUPDATEHANDLER_H
