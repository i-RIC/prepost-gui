#include "../graph2dhybridwindowresultsetting.h"
#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowbaseiterativeresultdataitem.h"
#include "graph2dhybridwindowresultcopydataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"

#include <guicore/postcontainer/postbaseiterativeseriesdatacontainer.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/stringtool.h>

#include <QColor>
#include <QStandardItem>
#include <QVector3D>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <cmath>
#include <qwt_plot_curve.h>

Graph2dHybridWindowBaseIterativeResultDataItem::Graph2dHybridWindowBaseIterativeResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent) :
	Graph2dHybridWindowResultDataItem(setting.name(), index, setting, parent)
{
	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();
	m_dataContainer = new PostBaseIterativeSeriesDataContainer(iRIC::toStr(setting.name()), projectData()->mainfile()->postSolutionInfo());
}

Graph2dHybridWindowBaseIterativeResultDataItem::~Graph2dHybridWindowBaseIterativeResultDataItem()
{
	delete m_dataContainer;
}

void Graph2dHybridWindowBaseIterativeResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void Graph2dHybridWindowBaseIterativeResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void Graph2dHybridWindowBaseIterativeResultDataItem::updateValues()
{
	m_xValues.clear();
	m_yValues.clear();

	int fn = 0;
	m_dataContainer->update(fn);
	auto timesteps = dataModel()->postSolutionInfo()->timeSteps()->timesteps();
	if (m_dataContainer->data().size() != timesteps.size()) {return;}

	m_xValues.assign(timesteps.count(), 0);
	m_yValues.assign(timesteps.count(), 0);

	for (int i = 0; i < timesteps.count(); ++i) {
		m_xValues[i] = timesteps.at(i);
		m_yValues[i] = m_dataContainer->data().at(i);
	}
}

Graph2dHybridWindowResultCopyDataItem* Graph2dHybridWindowBaseIterativeResultDataItem::copy(Graph2dHybridWindowResultCopyGroupDataItem* p)
{
	Graph2dHybridWindowResultCopyDataItem* ret = Graph2dHybridWindowResultDataItem::copy(p);

	QStringList args;
	args.append(m_standardItem->text());
	ret->setId(Graph2dHybridWindowResultCopyDataItem::idtTimeBaseIterative, args);

	return ret;
}
