#include "../graph2dhybridwindow.h"
#include "../graph2dhybridwindowcontrolwidget.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowgridpointresultdataitem.h"
#include "graph2dhybridwindowresultcopydataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"

#include <cmath>
#include <qwt_plot_curve.h>

#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/postcontainer/postzonepointseriesdatacontainer.h>
#include <misc/stringtool.h>

#include <QPen>
#include <QStandardItem>
#include <QVector3D>

#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

Graph2dHybridWindowGridPointResultDataItem::Graph2dHybridWindowGridPointResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent)
	: Graph2dHybridWindowResultDataItem(setting.name(), index, setting, parent)
{
	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();
	m_dataContainer = new PostZonePointSeriesDataContainer(info->dimension, info->zoneName, setting.name(), s.gridIndex(), this);
}

Graph2dHybridWindowGridPointResultDataItem::~Graph2dHybridWindowGridPointResultDataItem()
{
}

void Graph2dHybridWindowGridPointResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dHybridWindowGridPointResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Graph2dHybridWindowGridPointResultDataItem::updateValues(int fn)
{
	m_xValues.clear();
	m_yValues.clear();
	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	m_dataContainer->setPointIndex(s.gridIndex());
	m_dataContainer->update(fn);
	QList<double> timesteps = dataModel()->postSolutionInfo()->timeSteps()->timesteps();
	const Graph2dHybridWindowResultSetting& setting = dataModel()->setting();

	if (m_dataContainer->data().count() > timesteps.count()){
		return;
	}
	m_xValues.fill(0, timesteps.count());
	m_yValues.fill(0, timesteps.count());

	for (int i = 0; i < timesteps.count(); ++i){
		m_xValues[i] = timesteps.at(i);
		m_yValues[i] = m_dataContainer->data().at(i);
	}
}

Graph2dHybridWindowResultCopyDataItem* Graph2dHybridWindowGridPointResultDataItem::copy(Graph2dHybridWindowResultCopyGroupDataItem* p)
{
	Graph2dHybridWindowResultCopyDataItem* ret = Graph2dHybridWindowResultDataItem::copy(p);

	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();

	PostSolutionInfo* postInfo = dataModel()->postSolutionInfo();
	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(dataModel()->mainWindow());
	Graph2dHybridWindowControlWidget* c = w->controlWidget();

	int currentStep = postInfo->currentStep();
	const QList<double>& timesteps = postInfo->timeSteps()->timesteps();
	double time;
	if (timesteps.count() == 0){
		time = 0;
	} else {
		if (currentStep < timesteps.count()){
			time = timesteps.at(currentStep);
		} else {
			time = 0;
		}
	}

	QStringList args;
	args.append(m_standardItem->text());
	if (info->dataType == Graph2dHybridWindowResultSetting::dtDim1DStructured){
		args.append(QString::number(c->iValue() + 1));
		ret->setId(Graph2dHybridWindowResultCopyDataItem::idtTime1D, args);
	} else if (info->dataType == Graph2dHybridWindowResultSetting::dtDim2DStructured) {
		args.append(QString::number(c->iValue() + 1));
		args.append(QString::number(c->jValue() + 1));
		ret->setId(Graph2dHybridWindowResultCopyDataItem::idtTime2D, args);
	} else if (info->dataType == Graph2dHybridWindowResultSetting::dtDim3DStructured){
		args.append(QString::number(c->iValue() + 1));
		args.append(QString::number(c->jValue() + 1));
		args.append(QString::number(c->kValue() + 1));
		ret->setId(Graph2dHybridWindowResultCopyDataItem::idtTime3D, args);
	} else {
		args.append(QString::number(c->indexValue() + 1));
		ret->setId(Graph2dHybridWindowResultCopyDataItem::idtTimeUnst, args);
	}
	return ret;
}
