#include "../graph2dverificationwindow.h"
#include "../graph2dverificationwindowcontrolwidget.h"
#include "../graph2dverificationwindowresultsetting.h"
#include "../graph2dverificationwindowview.h"
#include "graph2dverificationwindowgridijkresultdataitem.h"
#include "graph2dverificationwindowresultcopydataitem.h"
#include "graph2dverificationwindowresultdataitem.h"
#include "graph2dverificationwindowresultgroupdataitem.h"

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

Graph2dVerificationWindowGridIJKResultDataItem::Graph2dVerificationWindowGridIJKResultDataItem(const Graph2dVerificationWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent)
	: Graph2dVerificationWindowResultDataItem(setting.name(), index, setting, parent)
{
	m_physVal = setting.name();
}

Graph2dVerificationWindowGridIJKResultDataItem::~Graph2dVerificationWindowGridIJKResultDataItem()
{
}

void Graph2dVerificationWindowGridIJKResultDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dVerificationWindowGridIJKResultDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Graph2dVerificationWindowGridIJKResultDataItem::updateValues(int /*fn*/)
{
	m_xValues.clear();
	m_yValues.clear();
	
	///{{{
	QVector<double> stdDist;

	QVector<double> distanceVals;
	QVector<double> calcVals;
	QVector<double> measuredVals;
	QVector<double> errorVals;

	double meanCalcVal = 0;
	double meanMeasuredVal = 0;

	double errorSquareSum = 0;

	// setup measuredVals.
	vtkPointSet* ps0 = postSolutionInfo()->zoneContainers2D().at(0)->data();

	PostSolutionInfo::Dimension dim = PostSolutionInfo::dim2D;
	std::string zoneName("iricZone");
	vtkPointSet* ps = postSolutionInfo()->zoneContainer(dim, zoneName)->data();

	//vtkPointSet* ps1 = m_setting.activePostData()->data();

	//vtkPolyData* pd = m_setting.activeMeasuredData()->pointData();
	//distanceVals.reserve(pd->GetNumberOfPoints());
	//measuredVals.reserve(pd->GetNumberOfPoints());
	//calcVals.reserve(pd->GetNumberOfPoints());
	//errorVals.reserve(pd->GetNumberOfPoints());
	//xVals.reserve(pd->GetNumberOfPoints());
	//yVals.reserve(pd->GetNumberOfPoints());
	//vtkDoubleArray* mvda = vtkDoubleArray::SafeDownCast(pd->GetPointData()->GetArray(iRIC::toStr(m_setting.activeValue()).c_str()));
	//vtkDoubleArray* crda = vtkDoubleArray::SafeDownCast(ps->GetPointData()->GetArray(iRIC::toStr(m_setting.activeResult()).c_str()));

	//vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);
	//if (sgrid != nullptr) {
	//	int dimensions[3];
	//	sgrid->GetDimensions(dimensions);
	//	int centerJ = dimensions[1] / 2;
	//	stdDist.reserve(dimensions[0]);
	//	double distance = 0;
	//	double point[3];
	//	QVector2D previousP, currentP;
	//	vtkIdType index = m_setting.activePostData()->nodeIndex(0, centerJ, 0);
	//	sgrid->GetPoint(index, point);
	//	previousP = QVector2D(point[0], point[1]);
	//	for (int i = 0; i < dimensions[0]; ++i) {
	//		index = m_setting.activePostData()->nodeIndex(i, centerJ, 0);
	//		sgrid->GetPoint(index, point);
	//		currentP = QVector2D(point[0], point[1]);
	//		distance += (currentP - previousP).length();
	//		stdDist.append(distance);
	//		previousP = currentP;
	//	}
	//}

	//for (vtkIdType i = 0; i < mvda->GetNumberOfTuples(); ++i) {
	//	double mval = mvda->GetValue(i);

	//	double point[3];
	//	pd->GetPoint(i, point);

	//	// find the cell that contains the measured value.
	//	vtkIdType cellid;
	//	double pcoords[4];
	//	double weights[4];
	//	int subid;
	//	cellid = ps->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
	//	if (cellid < 0) {
	//		// this point is outside of the calculation result.
	//		continue;
	//	}
	//	vtkCell* cell = ps->GetCell(cellid);
	//	double cval = 0;
	//	for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
	//		vtkIdType vid = cell->GetPointId(j);
	//		cval += *(weights + j) * crda->GetValue(vid);
	//	}
	//	measuredVals.append(mval);
	//	calcVals.append(cval);
	//	errorVals.append(cval - mval);
	//	xVals.push_back(point[0]);
	//	yVals.push_back(point[1]);

	//	errorSquareSum += (cval - mval) * (cval - mval);
	//	meanCalcVal += cval;
	//	meanMeasuredVal += mval;

	//	if (sgrid != nullptr) {
	//		int i, j, k;
	//		double dist = 0;
	//		for (int l = 0; l < cell->GetNumberOfPoints(); ++l) {
	//			vtkIdType vid = cell->GetPointId(l);
	//			m_setting.activePostData()->getNodeIJKIndex(vid, &i, &j, &k);
	//			double tmpdist = stdDist.at(i);
	//			dist += *(weights + l) * tmpdist;
	//		}
	//		distanceVals.append(dist);
	//	}
	//}
	//double rmse = sqrt(errorSquareSum / measuredVals.count());
	//meanCalcVal /= measuredVals.count();
	//meanMeasuredVal /= measuredVals.count();

	//double Sxy = 0;
	//double Sxx = 0;
	//for (vtkIdType i = 0; i < measuredVals.count(); ++i) {
	//	Sxy += (measuredVals.at(i) - meanMeasuredVal) * (calcVals.at(i) - meanCalcVal);
	//	Sxx += (measuredVals.at(i) - meanMeasuredVal) * (measuredVals.at(i) - meanMeasuredVal);
	//}
	//double b = Sxy / Sxx;
	//double a = meanCalcVal - b * meanMeasuredVal;

	//QVector<double> linex;
	//QVector<double> liney;
	//double xmin = 0;
	//double xmax = 0;
	//for (int i = 0; i < measuredVals.count(); ++i) {
	//	if (i == 0 || xmin > measuredVals.at(i)) { xmin = measuredVals.at(i); }
	//	if (i == 0 || xmax < measuredVals.at(i)) { xmax = measuredVals.at(i); }
	//}
	//double width = xmax - xmin;
	//double x = xmin - width * 0.1;
	//double y = a + b * x;
	//linex.append(x);
	//liney.append(y);
	//x = xmax + width * 0.1;
	//y = a + b * x;
	//linex.append(x);
	//liney.append(y);

	//QString lineTitle = QString("y = %1 * x").arg(b);
	//if (a > 0) {
	//	lineTitle.append(QString(" + %1").arg(a));
	//}
	//else {
	//	lineTitle.append(QString(" - %1").arg(-a));
	//}

	//QString comment;
	//comment = tr("Root Mean Squared Error: %1").arg(rmse);
	//QPen dotLinePen;
	//dotLinePen.setStyle(Qt::DotLine);
	//dotLinePen.setColor(Qt::gray);

	//m_zeroMarker->detach();

	//QBrush brush(Qt::gray);
	//QPen pen(Qt::black);
	//QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse, brush, pen, QSize(6, 6));

	////m_pointsCurve = new QwtPlotCustomCurve();
	////m_pointsCurve->setSymbol(symbol);
	////m_pointsCurve->setStyle(QwtPlotCurve::NoCurve);
	////m_pointsCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
	////m_pointsCurve->setLegendIconSize(QSize(7, 7));

	//m_lineCurve = new QwtPlotCustomCurve();
	//m_lineCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine);

	////{{
	//view()->setAxisAutoScale(QwtPlot::yLeft);
	//view()->setAxisAutoScale(QwtPlot::xBottom);
	/////view()->setMinimumSize(100, 100);
	////}}
	//switch (m_setting.m_graphType) {
	//case Graph2dVerificationWindowResultSetting::gtSWDvsValues:
	//	//m_pointsCurve->setSamples(distanceVals, measuredVals);
	//	//m_pointsCurve->setTitle(tr("Measured Values (%1)").arg(m_setting.activeValue()));
	//	//m_pointsCurve->attach(view());

	//	m_lineCurve->setSamples(distanceVals, calcVals);
	//	m_lineCurve->setTitle(tr("Calculation Result (%1)").arg(m_setting.activeResult()));
	//	m_lineCurve->attach(view());

	//	view()->setAxisTitle(QwtPlot::xBottom, tr("Stream-wise Distance"));
	//	view()->setAxisTitle(QwtPlot::yLeft, tr("Values"));
	//	break;
	//case Graph2dVerificationWindowResultSetting::gtSWDvsError:
	//	//m_pointsCurve->setSamples(distanceVals, errorVals);
	//	//m_pointsCurve->setTitle(tr("Residual Error"));
	//	//m_pointsCurve->attach(view());
	//	m_zeroMarker->attach(view());

	//	view()->setAxisTitle(QwtPlot::xBottom, tr("Stream-wise Distance"));
	//	view()->setAxisTitle(QwtPlot::yLeft, tr("Residual Error"));
	//	break;

	//case Graph2dVerificationWindowResultSetting::gtMVvsCR:
	//	//m_pointsCurve->setSamples(measuredVals, calcVals);
	//	//m_pointsCurve->setTitle(tr("Calculation Result (%1)").arg(m_setting.activeResult()));
	//	//m_pointsCurve->attach(view());

	//	m_lineCurve->setSamples(linex, linex);
	//	m_lineCurve->setTitle("y = x");
	//	m_lineCurve->attach(view());

	//	view()->setAxisTitle(QwtPlot::xBottom, tr("Measured Value (%1)").arg(m_setting.activeValue()));
	//	view()->setAxisTitle(QwtPlot::yLeft, tr("Calculation Result (%1)").arg(m_setting.activeResult()));
	//	break;

	//case Graph2dVerificationWindowResultSetting::gtMVvsError:
	//	//m_pointsCurve->setSamples(measuredVals, errorVals);
	//	//m_pointsCurve->setTitle(tr("Residual Error"));
	//	//m_pointsCurve->attach(view());
	//	m_zeroMarker->attach(view());

	//	view()->setAxisTitle(QwtPlot::xBottom, tr("Measured Value (%1)").arg(m_setting.activeValue()));
	//	view()->setAxisTitle(QwtPlot::yLeft, tr("Residual Error"));
	//	break;
	//}

	//view()->replot();

	//Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(mainWindow());
	//w->setComment(comment);
	///}}}


//	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
//#if 0 || 1
//	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();
//#endif
//
//	PostSolutionInfo* postInfo = dataModel()->postSolutionInfo();
//#if 0 || 1
//	PostZoneDataContainer* cont = postInfo->zoneContainer(info->dimension, info->zoneName);
//	if (cont == 0) {return;}
//
//	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(cont->data());
//	int dimension[3];
//	grid->GetDimensions(dimension);
//
//	vtkSmartPointer<vtkExtractGrid> extract = vtkSmartPointer<vtkExtractGrid>::New();
//	extract->SetInputData(grid);
//	switch (s.xAxisMode()) {
//	case Graph2dVerificationWindowResultSetting::xaI:
//		extract->SetVOI(0, dimension[0], s.gridJ(), s.gridJ(), s.gridK(), s.gridK());
//		break;
//	case Graph2dVerificationWindowResultSetting::xaJ:
//		extract->SetVOI(s.gridI(), s.gridI(), 0, dimension[1], s.gridK(), s.gridK());
//		break;
//	case Graph2dVerificationWindowResultSetting::xaK:
//		extract->SetVOI(s.gridI(), s.gridI(), s.gridJ(), s.gridJ(), 0, dimension[2]);
//		break;
//	default:
//		break;
//	}
//	extract->Update();
//
//	vtkSmartPointer<vtkStructuredGrid> extractedGrid = extract->GetOutput();
//	if (info->gridLocation == Vertex) {
//		updateValuesVertex(extractedGrid);
//	} else if (info->gridLocation == CellCenter) {
//		updateValuesCellCenter(extractedGrid);
//	} else {
//		Q_ASSERT(false);   //   Unhandled GridLocation_t
//	}
//#endif
}

//void Graph2dVerificationWindowGridIJKResultDataItem::updateValuesVertex(vtkStructuredGrid* extractedGrid)
//{
//	vtkDataArray* da = extractedGrid->GetPointData()->GetArray(iRIC::toStr(m_physVal).c_str());
//	if (da == 0) {
//		// no data found.
//		return;
//	}
//	int numT = da->GetNumberOfTuples();
//	Q_ASSERT(extractedGrid->GetNumberOfPoints() == numT);
//
//	m_xValues.fill(0, numT);
//	m_yValues.fill(0, numT);
//
//	double distance = 0;
//	double oldp[3];
//	for (int i = 0; i < numT; ++i) {
//		// X value setting
//		double p[3];
//		extractedGrid->GetPoint(i, p);
//		if (i == 0) {
//			oldp[0] = p[0];
//			oldp[1] = p[1];
//			oldp[2] = p[2];
//		} else {
//			QVector3D d(p[0] - oldp[0], p[1] - oldp[1], p[2] - oldp[2]);
//			distance = distance + d.length();
//		}
//		// x value
//		m_xValues[i] = distance;
//		// y value
//		double value = 0;
//		if (da->GetNumberOfComponents() == 1) {
//			value = da->GetTuple1(i);
//		} else if (da->GetNumberOfComponents() == 3) {
//			double* v;
//			v = da->GetTuple3(i);
//			value = std::sqrt(*v * *v + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
//		}
//		m_yValues[i] = value;
//		oldp[0] = p[0];
//		oldp[1] = p[1];
//		oldp[2] = p[2];
//	}
//}

//void Graph2dVerificationWindowGridIJKResultDataItem::updateValuesCellCenterStepWise(vtkStructuredGrid* extractedGrid)
//{
//	vtkDataArray* da = extractedGrid->GetCellData()->GetArray(iRIC::toStr(m_physVal).c_str());
//	if (da == 0) {
//		// no data found.
//		return;
//	}
//	int numT = da->GetNumberOfTuples();
//	Q_ASSERT(extractedGrid->GetNumberOfPoints() - 1 == numT);
//	m_xValues.clear(); m_xValues.reserve(numT * 2);
//	m_yValues.clear(); m_yValues.reserve(numT * 2);
//
//	double distance = 0;
//	double oldp[3];
//	double previous_value;
//	int npts = extractedGrid->GetNumberOfPoints();
//	for (int i = 0; i < npts; ++i) {
//		// X value setting
//		double p[3];
//		extractedGrid->GetPoint(i, p);
//		if (i == 0) {
//			oldp[0] = p[0];
//			oldp[1] = p[1];
//			oldp[2] = p[2];
//		} else {
//			QVector3D d(p[0] - oldp[0], p[1] - oldp[1], p[2] - oldp[2]);
//			distance = distance + d.length();
//		}
//
//		// y value
//		double value = 0;
//		if (da->GetNumberOfComponents() == 1) {
//			value = da->GetTuple1(i);
//		} else if (da->GetNumberOfComponents() == 3) {
//			double* v;
//			v = da->GetTuple3(i);
//			value = std::sqrt(*v * *v + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
//		}
//
//		if (i == 0) {
//			m_xValues.append(distance); m_yValues.append(value);
//		} else if (i == npts - 1) {
//			m_xValues.append(distance); m_yValues.append(previous_value);
//		} else {
//			m_xValues.append(distance); m_yValues.append(previous_value);
//			m_xValues.append(distance); m_yValues.append(value);
//		}
//
//		previous_value = value;
//		oldp[0] = p[0];
//		oldp[1] = p[1];
//		oldp[2] = p[2];
//	}
//}

//void Graph2dVerificationWindowGridIJKResultDataItem::updateValuesCellCenter(vtkStructuredGrid* extractedGrid)
//{
//	vtkDataArray* da = extractedGrid->GetCellData()->GetArray(iRIC::toStr(m_physVal).c_str());
//	if (da == 0) {
//		// no data found.
//		return;
//	}
//	int numT = da->GetNumberOfTuples();
//	Q_ASSERT(extractedGrid->GetNumberOfPoints() - 1 == numT);
//	m_xValues.clear(); m_xValues.reserve(numT);
//	m_yValues.clear(); m_yValues.reserve(numT);
//
//	double distance = 0;
//	double oldp[3];
//	extractedGrid->GetPoint(0, oldp);
//	for (int i = 0; i < numT; ++i) {
//		// X value setting
//		double p[3];
//		extractedGrid->GetPoint(i + 1, p);
//		QVector3D d(p[0] - oldp[0], p[1] - oldp[1], p[2] - oldp[2]);
//		distance = distance + d.length() / 2;
//
//		// y value
//		double value = 0;
//		if (da->GetNumberOfComponents() == 1) {
//			value = da->GetTuple1(i);
//		}
//		else if (da->GetNumberOfComponents() == 3) {
//			double* v;
//			v = da->GetTuple3(i);
//			value = std::sqrt(*v * *v + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
//		}
//		m_xValues.append(distance); m_yValues.append(value);
//
//		oldp[0] = p[0];
//		oldp[1] = p[1];
//		oldp[2] = p[2];
//	}
//}


Graph2dVerificationWindowResultCopyDataItem* Graph2dVerificationWindowGridIJKResultDataItem::copy(Graph2dVerificationWindowResultCopyGroupDataItem* p)
{
	Graph2dVerificationWindowResultCopyDataItem* ret = Graph2dVerificationWindowResultDataItem::copy(p);

	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = s.targetDataTypeInfo();

	PostSolutionInfo* postInfo = dataModel()->postSolutionInfo();
	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(dataModel()->mainWindow());
	Graph2dVerificationWindowControlWidget* c = w->controlWidget();

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
	if (info->dataType == Graph2dVerificationWindowResultSetting::dtDim1DStructured) {
		ret->setId(Graph2dVerificationWindowResultCopyDataItem::idt1D, args);
	} else if (info->dataType == Graph2dVerificationWindowResultSetting::dtDim2DStructured) {
		if (s.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			args.append(QString::number(c->jValue() + 1));
			ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtI2D, args);
		} else {
			args.append(QString::number(c->iValue() + 1));
			ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtJ2D, args);
		}
	} else if (info->dataType == Graph2dVerificationWindowResultSetting::dtDim3DStructured) {
		if (s.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			args.append(QString::number(c->jValue() + 1));
			args.append(QString::number(c->kValue() + 1));
			ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtI3D, args);
		} else if (s.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
			args.append(QString::number(c->iValue() + 1));
			args.append(QString::number(c->kValue() + 1));
			ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtJ3D, args);
		} else {
			args.append(QString::number(c->iValue() + 1));
			args.append(QString::number(c->jValue() + 1));
			ret->setId(Graph2dVerificationWindowResultCopyDataItem::idtK3D, args);
		}
	}
#endif
	return ret;
}
