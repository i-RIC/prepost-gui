#include "../graph2dhybridwindow.h"
#include "../graph2dhybridwindowcontrolwidget.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowgridijkresultdataitem.h"
#include "graph2dhybridwindowresultcopydataitem.h"
#include "graph2dhybridwindowresultdataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"

#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>
#include <misc/stringtool.h>

#include <QPen>
#include <QStandardItem>
#include <QVector3D>

#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

#include <cmath>
#include <qwt_plot_curve.h>

Graph2dHybridWindowGridIJKResultDataItem::Graph2dHybridWindowGridIJKResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent)
	: Graph2dHybridWindowResultDataItem(setting.name(), index, setting, parent)
{
	m_physVal = setting.name();
}

Graph2dHybridWindowGridIJKResultDataItem::~Graph2dHybridWindowGridIJKResultDataItem()
{
}

void Graph2dHybridWindowGridIJKResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dHybridWindowGridIJKResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Graph2dHybridWindowGridIJKResultDataItem::updateValues(int /*fn*/)
{
	m_xValues.clear();
	m_yValues.clear();

	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();

	PostSolutionInfo* postInfo = dataModel()->postSolutionInfo();
	PostZoneDataContainer* cont = postInfo->zoneContainer(info->dimension, info->zoneName);
	if (cont == 0) {return;}

	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(cont->data());
	int dimension[3];
	grid->GetDimensions(dimension);

	vtkSmartPointer<vtkExtractGrid> extract = vtkSmartPointer<vtkExtractGrid>::New();
	extract->SetInputData(grid);
	switch (s.xAxisMode()) {
	case Graph2dHybridWindowResultSetting::xaI:
		extract->SetVOI(0, dimension[0], s.gridJ(), s.gridJ(), s.gridK(), s.gridK());
		break;
	case Graph2dHybridWindowResultSetting::xaJ:
		extract->SetVOI(s.gridI(), s.gridI(), 0, dimension[1], s.gridK(), s.gridK());
		break;
	case Graph2dHybridWindowResultSetting::xaK:
		extract->SetVOI(s.gridI(), s.gridI(), s.gridJ(), s.gridJ(), 0, dimension[2]);
		break;
	default:
		break;
	}
	extract->Update();

	vtkSmartPointer<vtkStructuredGrid> extractedGrid = extract->GetOutput();
	if (info->gridLocation == Vertex) {
		updateValuesVertex(extractedGrid);
	} else if (info->gridLocation == CellCenter) {
		updateValuesCellCenter(extractedGrid);
	} else {
		Q_ASSERT(false);   //   Unhandled GridLocation_t
	}
}

void Graph2dHybridWindowGridIJKResultDataItem::updateValuesVertex(vtkStructuredGrid* extractedGrid)
{
	vtkDataArray* da = extractedGrid->GetPointData()->GetArray(iRIC::toStr(m_physVal).c_str());
	if (da == 0) {
		// no data found.
		return;
	}
	int numT = da->GetNumberOfTuples();
	Q_ASSERT(extractedGrid->GetNumberOfPoints() == numT);

	m_xValues.fill(0, numT);
	m_yValues.fill(0, numT);

	double distance = 0;
	double oldp[3];
	for (int i = 0; i < numT; ++i) {
		// X value setting
		double p[3];
		extractedGrid->GetPoint(i, p);
		if (i == 0) {
			oldp[0] = p[0];
			oldp[1] = p[1];
			oldp[2] = p[2];
		} else {
			QVector3D d(p[0] - oldp[0], p[1] - oldp[1], p[2] - oldp[2]);
			distance = distance + d.length();
		}
		// x value
		m_xValues[i] = distance;
		// y value
		double value = 0;
		if (da->GetNumberOfComponents() == 1) {
			value = da->GetTuple1(i);
		} else if (da->GetNumberOfComponents() == 3) {
			double* v;
			v = da->GetTuple3(i);
			value = std::sqrt(*v * *v + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
		}
		m_yValues[i] = value;
		oldp[0] = p[0];
		oldp[1] = p[1];
		oldp[2] = p[2];
	}
}

void Graph2dHybridWindowGridIJKResultDataItem::updateValuesCellCenterStepWise(vtkStructuredGrid* extractedGrid)
{
	vtkDataArray* da = extractedGrid->GetCellData()->GetArray(iRIC::toStr(m_physVal).c_str());
	if (da == 0) {
		// no data found.
		return;
	}
	int numT = da->GetNumberOfTuples();
	Q_ASSERT(extractedGrid->GetNumberOfPoints() - 1 == numT);
	m_xValues.clear(); m_xValues.reserve(numT * 2);
	m_yValues.clear(); m_yValues.reserve(numT * 2);

	double distance = 0;
	double oldp[3];
	double previous_value;
	int npts = extractedGrid->GetNumberOfPoints();
	for (int i = 0; i < npts; ++i) {
		// X value setting
		double p[3];
		extractedGrid->GetPoint(i, p);
		if (i == 0) {
			oldp[0] = p[0];
			oldp[1] = p[1];
			oldp[2] = p[2];
		} else {
			QVector3D d(p[0] - oldp[0], p[1] - oldp[1], p[2] - oldp[2]);
			distance = distance + d.length();
		}

		// y value
		double value = 0;
		if (da->GetNumberOfComponents() == 1) {
			value = da->GetTuple1(i);
		} else if (da->GetNumberOfComponents() == 3) {
			double* v;
			v = da->GetTuple3(i);
			value = std::sqrt(*v * *v + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
		}

		if (i == 0) {
			m_xValues.append(distance); m_yValues.append(value);
		} else if (i == npts - 1) {
			m_xValues.append(distance); m_yValues.append(previous_value);
		} else {
			m_xValues.append(distance); m_yValues.append(previous_value);
			m_xValues.append(distance); m_yValues.append(value);
		}

		previous_value = value;
		oldp[0] = p[0];
		oldp[1] = p[1];
		oldp[2] = p[2];
	}
}

void Graph2dHybridWindowGridIJKResultDataItem::updateValuesCellCenter(vtkStructuredGrid* extractedGrid)
{
	vtkDataArray* da = extractedGrid->GetCellData()->GetArray(iRIC::toStr(m_physVal).c_str());
	if (da == 0) {
		// no data found.
		return;
	}
	int numT = da->GetNumberOfTuples();
	Q_ASSERT(extractedGrid->GetNumberOfPoints() - 1 == numT);
	m_xValues.clear(); m_xValues.reserve(numT);
	m_yValues.clear(); m_yValues.reserve(numT);

	double distance = 0;
	double oldp[3];
	extractedGrid->GetPoint(0, oldp);
	for (int i = 0; i < numT; ++i) {
		// X value setting
		double p[3];
		extractedGrid->GetPoint(i + 1, p);
		QVector3D d(p[0] - oldp[0], p[1] - oldp[1], p[2] - oldp[2]);
		distance = distance + d.length() / 2;

		// y value
		double value = 0;
		if (da->GetNumberOfComponents() == 1) {
			value = da->GetTuple1(i);
		}
		else if (da->GetNumberOfComponents() == 3) {
			double* v;
			v = da->GetTuple3(i);
			value = std::sqrt(*v * *v + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
		}
		m_xValues.append(distance); m_yValues.append(value);

		oldp[0] = p[0];
		oldp[1] = p[1];
		oldp[2] = p[2];
	}
}


Graph2dHybridWindowResultCopyDataItem* Graph2dHybridWindowGridIJKResultDataItem::copy(Graph2dHybridWindowResultCopyGroupDataItem* p)
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
	if (timesteps.count() == 0) {
		time = 0;
	} else {
		if (currentStep < timesteps.count()) {
			time = timesteps.at(currentStep);
		} else {
			time = 0;
		}
	}

	QStringList args;
	args.append(m_standardItem->text());
	args.append(QString::number(time));
	if (info->dataType == Graph2dHybridWindowResultSetting::dtDim1DStructured) {
		ret->setId(Graph2dHybridWindowResultCopyDataItem::idt1D, args);
	} else if (info->dataType == Graph2dHybridWindowResultSetting::dtDim2DStructured) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			args.append(QString::number(c->jValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtI2D, args);
		} else {
			args.append(QString::number(c->iValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtJ2D, args);
		}
	} else if (info->dataType == Graph2dHybridWindowResultSetting::dtDim3DStructured) {
		if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			args.append(QString::number(c->jValue() + 1));
			args.append(QString::number(c->kValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtI3D, args);
		} else if (s.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			args.append(QString::number(c->iValue() + 1));
			args.append(QString::number(c->kValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtJ3D, args);
		} else {
			args.append(QString::number(c->iValue() + 1));
			args.append(QString::number(c->jValue() + 1));
			ret->setId(Graph2dHybridWindowResultCopyDataItem::idtK3D, args);
		}
	}
	return ret;
}
