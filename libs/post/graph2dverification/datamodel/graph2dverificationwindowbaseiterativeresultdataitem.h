#ifndef GRAPH2DVERIFICATIONWINDOWBASEITERATIVERESULTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWBASEITERATIVERESULTDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"
#include "graph2dverificationwindowresultdataitem.h"

class PostBaseIterativeSeriesDataContainer;
class QColor;

class Graph2dVerificationWindowBaseIterativeResultDataItem : public Graph2dVerificationWindowResultDataItem
{

public:
	Graph2dVerificationWindowBaseIterativeResultDataItem(const Graph2dVerificationWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	~Graph2dVerificationWindowBaseIterativeResultDataItem();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	Graph2dVerificationWindowResultCopyDataItem* copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent) override;

protected:
	void updateValues(int fn) override;

private:
	PostBaseIterativeSeriesDataContainer* m_dataContainer;
};

#endif // GRAPH2DVERIFICATIONWINDOWBASEITERATIVERESULTDATAITEM_H
