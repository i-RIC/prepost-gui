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
	/// Constructor
	Graph2dHybridWindowRootDataItem(Graph2dHybridWindow* window, ProjectDataItem* parent);
	/// Destructor
	~Graph2dHybridWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
	Graph2dHybridWindowResultGroupDataItem* resultGroupItem() {
		return m_resultGroupItem;
	}
	Graph2dHybridWindowResultCopyGroupDataItem* resultCopyGroupItem() {
		return m_resultCopyGroupItem;
	}
	Graph2dHybridWindowImportDataGroupDataItem* importDataGroupItem() {
		return m_importDataGroupItem;
	}
	Graph2dWindowMarkerGroupDataItem* markerGroupItem() {
		return m_markerGroupItem;
	}
	void updateData(int fn);
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;

signals:
	void standardModelSetuped();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	Graph2dHybridWindowResultGroupDataItem* m_resultGroupItem;
	Graph2dHybridWindowResultCopyGroupDataItem* m_resultCopyGroupItem;
	Graph2dHybridWindowImportDataGroupDataItem* m_importDataGroupItem;
	Graph2dWindowMarkerGroupDataItem* m_markerGroupItem;

public:
	friend class Graph2dHybridWindowProjectDataItem;
	friend class Graph2dHybridWindowDataModel;
};

#endif // GRAPH2DHYBRIDWINDOWROOTDATAITEM_H
