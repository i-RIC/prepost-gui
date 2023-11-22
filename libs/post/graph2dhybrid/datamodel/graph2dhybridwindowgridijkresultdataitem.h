#ifndef GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "graph2dhybridwindowresultdataitem.h"

class vtkStructuredGrid;

class Graph2dHybridWindowGridIJKResultDataItem : public Graph2dHybridWindowResultDataItem
{

public:
	Graph2dHybridWindowGridIJKResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowGridIJKResultDataItem() override;

	Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues() override;

private:
	QString m_physVal;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H
