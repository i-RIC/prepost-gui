#ifndef GRIDBIRDEYEWINDOWROOTDATAITEM_H
#define GRIDBIRDEYEWINDOWROOTDATAITEM_H

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include "../gridbirdeyewindowdatamodel.h"
#include <QList>

class Axis3dDataItem;
class GridBirdEyeWindow;
class GridBirdEyeWindowZoneDataItem;

class QStandardItemModel;

class GridBirdEyeWindowRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowRootDataItem(GridBirdEyeWindow* window, ProjectDataItem* parent);
	~GridBirdEyeWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
	GridBirdEyeWindowZoneDataItem* zoneDataItem() const;

private slots:
	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GridBirdEyeWindowZoneDataItem* m_zoneDataItem;
	Axis3dDataItem* m_axesDataItem;

public:
	friend class GridBirdEyeWindowProjectDataItem;
	friend class GridBirdEyeWindowDataModel;
};

#endif // GRIDBIRDEYEWINDOWROOTDATAITEM_H
