#include "../graph2dverificationwindowresultsetting.h"
#include "../graph2dverificationwindowview.h"
#include "graph2dverificationwindowbaseiterativeresultdataitem.h"
#include "graph2dverificationwindowresultcopydataitem.h"
#include "graph2dverificationwindowresultgroupdataitem.h"

#include <guicore/postcontainer/postbaseiterativeseriesdatacontainer.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <misc/stringtool.h>

#include <QColor>
#include <QStandardItem>
#include <QVector3D>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <cmath>
#include <qwt_plot_curve.h>

Graph2dVerificationWindowBaseIterativeResultDataItem::Graph2dVerificationWindowBaseIterativeResultDataItem(const Graph2dVerificationWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent) :
	Graph2dVerificationWindowResultDataItem(setting.name(), index, setting, parent)
{
	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();
	m_dataContainer = new PostBaseIterativeSeriesDataContainer(info->dimension, setting.name(), this);
#endif
}

Graph2dVerificationWindowBaseIterativeResultDataItem::~Graph2dVerificationWindowBaseIterativeResultDataItem()
{}

void Graph2dVerificationWindowBaseIterativeResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void Graph2dVerificationWindowBaseIterativeResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void Graph2dVerificationWindowBaseIterativeResultDataItem::updateValues(int fn)
{
	m_xValues.clear();
	m_yValues.clear();

	m_dataContainer->update(fn);
	QList<double> timesteps = dataModel()->postSolutionInfo()->timeSteps()->timesteps();
	if (m_dataContainer->data().count() != timesteps.count()) {return;}

	m_xValues.fill(0, timesteps.count());
	m_yValues.fill(0, timesteps.count());

	for (int i = 0; i < timesteps.count(); ++i) {
		m_xValues[i] = timesteps.at(i);
		m_yValues[i] = m_dataContainer->data().at(i);
	}
}

Graph2dVerificationWindowResultCopyDataItem* Graph2dVerificationWindowBaseIterativeResultDataItem::copy(Graph2dVerificationWindowResultCopyGroupDataItem* p)
{
	Graph2dVerificationWindowResultCopyDataItem* ret = Graph2dVerificationWindowResultDataItem::copy(p);

	QStringList args;
	args.append(m_standardItem->text());
	ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtTimeBaseIterative, args);

	return ret;
}
