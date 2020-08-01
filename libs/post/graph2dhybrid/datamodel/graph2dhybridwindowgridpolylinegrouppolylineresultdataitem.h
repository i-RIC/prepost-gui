#ifndef GRAPH2DHYBRIDWINDOWGRIDPOLYLINEGROUPPOLYLINERESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDPOLYLINEGROUPPOLYLINERESULTDATAITEM_H

#include "graph2dhybridwindowgridabstractpolylineresultdataitem.h"

class Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem : public Graph2dHybridWindowGridAbstractPolylineResultDataItem
{
public:
	Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowGridPolyLineGroupPolyLineResultDataItem() override;

private:
	std::vector<QPointF> getPolyLine() override;

	std::vector<QMetaObject::Connection> m_connections;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDPOLYLINEGROUPPOLYLINERESULTDATAITEM_H
