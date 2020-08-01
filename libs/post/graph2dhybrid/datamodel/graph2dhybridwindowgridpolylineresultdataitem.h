#ifndef GRAPH2DHYBRIDWINDOWGRIDPOLYLINERESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDPOLYLINERESULTDATAITEM_H

#include "graph2dhybridwindowgridabstractpolylineresultdataitem.h"

class Graph2dHybridWindowGridPolylineResultDataItem : public Graph2dHybridWindowGridAbstractPolylineResultDataItem
{
public:
	Graph2dHybridWindowGridPolylineResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowGridPolylineResultDataItem() override;

private:
	std::vector<QPointF> getPolyLine() override;

	std::vector<QMetaObject::Connection> m_connections;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDPOLYLINERESULTDATAITEM_H
