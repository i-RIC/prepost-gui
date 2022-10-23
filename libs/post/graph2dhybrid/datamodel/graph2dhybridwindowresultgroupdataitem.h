#ifndef GRAPH2DHYBRIDWINDOWRESULTGROUPDATAITEM_H
#define GRAPH2DHYBRIDWINDOWRESULTGROUPDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"
#include <post/graph2d/graph2dwindowdatamodel.h>
#include <QList>
#include <QMap>

class QwtPlotMarker;
class QAction;
class SolverDefinitionGridType;
class Post2dWindowZoneDataItem;
class Graph2dHybridWindowHorizontalAxisSetting;
class Graph2dHybridWindowResultCopyGroupDataItem;

class Graph2dHybridWindowResultGroupDataItem : public Graph2dHybridWindowDataItem
{
	Q_OBJECT

public:
	Graph2dHybridWindowResultGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowResultGroupDataItem();

	void updateData();
	bool setupInitialSetting();
	void applyHorizontalAxisSettings();
	void getAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max);
	void showHorizontalAxisSettingDialog();
	Graph2dHybridWindowHorizontalAxisSetting* horizontalAxisSetting();
	void updateChildren(const Graph2dHybridWindowResultSetting& setting);
	void copy(Graph2dHybridWindowResultCopyGroupDataItem* parent);
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const ;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void applySettings();
	Graph2dHybridWindowHorizontalAxisSetting* m_hAxisSetting;
};

#endif // GRAPH2DHYBRIDWINDOWRESULTGROUPDATAITEM_H
