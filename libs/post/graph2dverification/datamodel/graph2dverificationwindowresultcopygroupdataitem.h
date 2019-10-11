#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWRESULTCOPYGROUPDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWRESULTCOPYGROUPDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"

class Graph2dVerificationWindowResultCopyDataItem;

class Graph2dVerificationWindowResultCopyGroupDataItem : public Graph2dVerificationWindowDataItem
{
	Q_OBJECT

public:
	Graph2dVerificationWindowResultCopyGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dVerificationWindowResultCopyGroupDataItem();
	void updateData();
	void addItem(Graph2dVerificationWindowResultCopyDataItem* item);
	void clear();
	bool axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // GRAPH2DVERIFICATIONWINDOWRESULTCOPYGROUPDATAITEM_H
#endif
