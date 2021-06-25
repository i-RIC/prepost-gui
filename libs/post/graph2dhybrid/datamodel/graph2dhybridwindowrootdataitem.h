#ifndef GRAPH2DHYBRIDWINDOWROOTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWROOTDATAITEM_H

#include <post/graph2d/graph2dwindowrootdataitem.h>
#include <post/graph2d/graph2dwindowdatamodel.h>
#include "../graph2dhybridwindowresultsetting.h"
#include <QList>
#include <QMap>

class Graph2dHybridWindowDataModel;
class QStandardItemModel;
class Graph2dHybridWindow;
class Graph2dHybridWindowResultGroupDataItem;
class Graph2dHybridWindowResultCopyGroupDataItem;
class Graph2dHybridWindowImportDataGroupDataItem;
class Graph2dWindowMarkerGroupDataItem;

class Graph2dHybridWindowRootDataItem : public Graph2dWindowRootDataItem
{
	Q_OBJECT

public:
	Graph2dHybridWindowRootDataItem(Graph2dHybridWindow* window, ProjectDataItem* parent);
	~Graph2dHybridWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
	Graph2dHybridWindowResultGroupDataItem* resultGroupItem() const;
	Graph2dHybridWindowResultCopyGroupDataItem* resultCopyGroupItem() const;
	Graph2dHybridWindowImportDataGroupDataItem* importDataGroupItem() const;
	Graph2dWindowMarkerGroupDataItem* markerGroupItem() const;

	void updateData();
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;

signals:
	void standardModelSetuped();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Graph2dHybridWindowResultGroupDataItem* m_resultGroupItem;
	Graph2dHybridWindowResultCopyGroupDataItem* m_resultCopyGroupItem;
	Graph2dHybridWindowImportDataGroupDataItem* m_importDataGroupItem;
	Graph2dWindowMarkerGroupDataItem* m_markerGroupItem;

public:
	friend class Graph2dHybridWindowProjectDataItem;
	friend class Graph2dHybridWindowDataModel;
};

#endif // GRAPH2DHYBRIDWINDOWROOTDATAITEM_H
