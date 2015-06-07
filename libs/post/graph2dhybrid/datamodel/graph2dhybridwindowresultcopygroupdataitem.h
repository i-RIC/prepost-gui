#ifndef GRAPH2DHYBRIDWINDOWRESULTCOPYGROUPDATAITEM_H
#define GRAPH2DHYBRIDWINDOWRESULTCOPYGROUPDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"

class Graph2dHybridWindowResultCopyDataItem;

class Graph2dHybridWindowResultCopyGroupDataItem : public Graph2dHybridWindowDataItem
{
	Q_OBJECT

public:
	Graph2dHybridWindowResultCopyGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowResultCopyGroupDataItem();
	void updateData();
	void addItem(Graph2dHybridWindowResultCopyDataItem* item);
	void clear();
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // GRAPH2DHYBRIDWINDOWRESULTCOPYGROUPDATAITEM_H
