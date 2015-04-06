#ifndef GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "graph2dhybridwindowresultdataitem.h"

class Graph2dHybridWindowGridIJKResultDataItem : public Graph2dHybridWindowResultDataItem
{
public:
	Graph2dHybridWindowGridIJKResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dHybridWindowGridIJKResultDataItem();
	Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent);
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void updateValues(int fn);
private:
	QString m_physVal;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H
