#if SKIP
#include "../graph2dverificationwindow.h"
#include "../graph2dverificationwindowcontrolwidget.h"
#include "../graph2dverificationwindowresultsetting.h"
#include "../graph2dverificationwindowview.h"
#include "graph2dverificationwindowgridpointresultdataitem.h"
#include "graph2dverificationwindowresultcopydataitem.h"
#include "graph2dverificationwindowresultgroupdataitem.h"

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

#include <cmath>
#include <qwt_plot_curve.h>

Graph2dVerificationWindowGridPointResultDataItem::Graph2dVerificationWindowGridPointResultDataItem(const Graph2dVerificationWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent) :
	Graph2dVerificationWindowResultDataItem(setting.name(), index, setting, parent)
{
	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();
	m_dataContainer = new PostZonePointSeriesDataContainer(info->dimension, info->zoneName, setting.name(), s.gridIndex(), info->gridLocation, postSolutionInfo());
#endif
}

Graph2dVerificationWindowGridPointResultDataItem::~Graph2dVerificationWindowGridPointResultDataItem()
{}

void Graph2dVerificationWindowGridPointResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void Graph2dVerificationWindowGridPointResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void Graph2dVerificationWindowGridPointResultDataItem::updateValues(int fn)
{
	m_xValues.clear();
	m_yValues.clear();
	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
	m_dataContainer->setPointIndex(s.gridIndex());
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();
	m_dataContainer->setGridLocation(info->gridLocation);
#endif
	m_dataContainer->update(fn);
	QList<double> timesteps = dataModel()->postSolutionInfo()->timeSteps()->timesteps();

	if (m_dataContainer->data().count() > timesteps.count()) {
		Q_ASSERT_X(false, "Graph2dVerificationWindowGridPointResultDataItem::updateValues", "FIX PostZonePointSeriesDataContainer::loadData");
		return;
	}
	m_xValues.fill(0, timesteps.count());
	m_yValues.fill(0, timesteps.count());

	for (int i = 0; i < timesteps.count(); ++i) {
		m_xValues[i] = timesteps.at(i);
		m_yValues[i] = m_dataContainer->data().at(i);
	}
}

Graph2dVerificationWindowResultCopyDataItem* Graph2dVerificationWindowGridPointResultDataItem::copy(Graph2dVerificationWindowResultCopyGroupDataItem* p)
{
	Graph2dVerificationWindowResultCopyDataItem* ret = Graph2dVerificationWindowResultDataItem::copy(p);

	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();
#endif

	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(dataModel()->mainWindow());
	Graph2dVerificationWindowControlWidget* c = w->controlWidget();

	QStringList args;
	args.append(m_standardItem->text());
#if 0 || 1
	if (info->dataType == Graph2dVerificationWindowResultSetting::dtDim1DStructured) {
		args.append(QString::number(c->iValue() + 1));
		ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtTime1D, args);
	} else if (info->dataType == Graph2dVerificationWindowResultSetting::dtDim2DStructured) {
		args.append(QString::number(c->iValue() + 1));
		args.append(QString::number(c->jValue() + 1));
		ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtTime2D, args);
	} else if (info->dataType == Graph2dVerificationWindowResultSetting::dtDim3DStructured) {
		args.append(QString::number(c->iValue() + 1));
		args.append(QString::number(c->jValue() + 1));
		args.append(QString::number(c->kValue() + 1));
		ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtTime3D, args);
	} else {
		args.append(QString::number(c->indexValue() + 1));
		ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtTimeUnst, args);
	}
#endif
	return ret;
}
#endif
