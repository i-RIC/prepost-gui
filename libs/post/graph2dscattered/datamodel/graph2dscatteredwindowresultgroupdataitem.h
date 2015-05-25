#ifndef GRAPH2DSCATTEREDWINDOWRESULTGROUPDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWRESULTGROUPDATAITEM_H

#include "../graph2dscatteredwindowdataitem.h"
#include "../graph2dscatteredwindowresultsetting.h"
#include <post/graph2d/graph2dwindowdatamodel.h>
#include <QList>
#include <QMap>

class Graph2dScatteredWindowResultGroupDataItem : public Graph2dScatteredWindowDataItem
{
	Q_OBJECT
public:
	Graph2dScatteredWindowResultGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dScatteredWindowResultGroupDataItem();
	void updateData(int fn);
	bool setupInitialSetting();
	void getAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max);
	void updateChildren(const Graph2dScatteredWindowResultSetting& setting);
	bool axisNeeded(Graph2dScatteredWindowResultSetting::AxisSide as) const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // GRAPH2DSCATTEREDWINDOWRESULTGROUPDATAITEM_H
