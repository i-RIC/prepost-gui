#include <cmath>

#include "graph2dscatteredwindowgridresultdataitem.h"
#include "graph2dscatteredwindowresultgroupdataitem.h"
#include <guicore/postcontainer/postzonedatacontainer.h>
#include "../graph2dscatteredwindowresultsetting.h"
#include <misc/stringtool.h>

#include <vtkPointSet.h>
#include <vtkPointData.h>

Graph2dScatteredWindowGridResultDataItem::Graph2dScatteredWindowGridResultDataItem(const Graph2dScatteredWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent)
	: Graph2dScatteredWindowResultDataItem(setting.name(), index, setting, parent)
{
	m_yAxis = setting.name();
}

Graph2dScatteredWindowGridResultDataItem::~Graph2dScatteredWindowGridResultDataItem()
{
}

void Graph2dScatteredWindowGridResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dScatteredWindowGridResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Graph2dScatteredWindowGridResultDataItem::updateValues(int fn)
{
	m_xValues.clear();
	m_yValues.clear();

	const Graph2dScatteredWindowResultSetting& s = dataModel()->setting();

	PostSolutionInfo* postInfo = dataModel()->postSolutionInfo();
	PostZoneDataContainer* cont = postInfo->zoneContainer(s.dimension(), s.zoneName());
	if (cont == 0){return;}

	vtkPointSet *ps = cont->data();

	m_xValues.reserve(ps->GetNumberOfPoints());
	m_yValues.reserve(ps->GetNumberOfPoints());

	// setup x value.
	setupData(m_xValues, s.xAxis(), ps);
	// setup y value.
	setupData(m_yValues, m_yAxis, ps);
}

void Graph2dScatteredWindowGridResultDataItem::setupData(QVector<double>& vals, const QString& name, vtkPointSet* ps)
{
	if (name == Graph2dScatteredWindowResultSetting::XAXIS_POSITION_X){
		for (vtkIdType id = 0; id < ps->GetNumberOfPoints(); ++id){
			double * v = ps->GetPoint(id);
			vals.append(*v);
		}
	} else if (name == Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Y){
		for (vtkIdType id = 0; id < ps->GetNumberOfPoints(); ++id){
			double * v = ps->GetPoint(id);
			vals.append(*(v + 1));
		}
	} else if (name == Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Z){
		for (vtkIdType id = 0; id < ps->GetNumberOfPoints(); ++id){
			double * v = ps->GetPoint(id);
			vals.append(*(v + 2));
		}
	} else {
		vtkPointData* pd = ps->GetPointData();
		vtkDataArray* da = pd->GetArray(iRIC::toStr(name).c_str());
		double tuple;
		for (vtkIdType id = 0; id < ps->GetNumberOfPoints(); ++id){
			da->GetTuple(id, &tuple);
			vals.append(tuple);
		}
	}
}
