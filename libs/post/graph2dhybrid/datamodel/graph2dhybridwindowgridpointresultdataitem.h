#ifndef GRAPH2DHYBRIDWINDOWGRIDPOINTRESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDPOINTRESULTDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "graph2dhybridwindowresultdataitem.h"
#include <guicore/postcontainer/postsolutioninfo.h>

class PostZonePointSeriesDataContainer;

class Graph2dHybridWindowGridPointResultDataItem : public Graph2dHybridWindowResultDataItem
{

public:
	Graph2dHybridWindowGridPointResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dHybridWindowGridPointResultDataItem();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent) override;

protected:
	void updateValues(int fn) override;

private:
	PostZonePointSeriesDataContainer* m_dataContainer;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDPOINTRESULTDATAITEM_H
