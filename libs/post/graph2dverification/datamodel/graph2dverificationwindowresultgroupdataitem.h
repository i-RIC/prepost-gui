#ifndef GRAPH2DVERIFICATIONWINDOWRESULTGROUPDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWRESULTGROUPDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"
#include <post/graph2d/graph2dwindowdatamodel.h>
#include <QList>
#include <QMap>

class QwtPlotMarker;
class QAction;
class SolverDefinitionGridType;
class Post2dWindowZoneDataItem;
class LookupTableContainer;
class Graph2dVerificationWindowHorizontalAxisSetting;
class Graph2dVerificationWindowResultCopyGroupDataItem;

class Graph2dVerificationWindowResultGroupDataItem : public Graph2dVerificationWindowDataItem
{
	Q_OBJECT

public:
	Graph2dVerificationWindowResultGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dVerificationWindowResultGroupDataItem();
	void updateData(int fn);
	bool setupInitialSetting();
	void applyHorizontalAxisSettings();
	void getAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max);
	void showHorizontalAxisSettingDialog();
	Graph2dVerificationWindowHorizontalAxisSetting* horizontalAxisSetting() {
		return m_hAxisSetting;
	}
	void updateChildren(const Graph2dVerificationWindowResultSetting& setting);
	void copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent);
	bool axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const ;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void applySettings();
	Graph2dVerificationWindowHorizontalAxisSetting* m_hAxisSetting;
};

#endif // GRAPH2DVERIFICATIONWINDOWRESULTGROUPDATAITEM_H
