#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWGRIDPOINTRESULTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWGRIDPOINTRESULTDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"
#include "graph2dverificationwindowresultdataitem.h"
#include <guicore/postcontainer/postsolutioninfo.h>

class PostZonePointSeriesDataContainer;

class Graph2dVerificationWindowGridPointResultDataItem : public Graph2dVerificationWindowResultDataItem
{

public:
	Graph2dVerificationWindowGridPointResultDataItem(const Graph2dVerificationWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dVerificationWindowGridPointResultDataItem();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	Graph2dVerificationWindowResultCopyDataItem* copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent) override;

protected:
	void updateValues(int fn) override;

private:
	PostZonePointSeriesDataContainer* m_dataContainer;
};

#endif // GRAPH2DVERIFICATIONWINDOWGRIDPOINTRESULTDATAITEM_H
#endif
