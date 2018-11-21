#include "graph2dhybridwindow.h"
#include "graph2dhybridwindowdataitem.h"
#include "graph2dhybridwindowdatamodel.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

Graph2dHybridWindow* Graph2dHybridWindowDataItem::graphWindow()
{
	return dynamic_cast<Graph2dHybridWindow*>(dataModel()->mainWindow());
}

PostSolutionInfo* Graph2dHybridWindowDataItem::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

Graph2dHybridWindowDataModel* Graph2dHybridWindowDataItem::dataModel()
{
	return dynamic_cast<Graph2dHybridWindowDataModel*>(Graph2dWindowDataItem::dataModel());
}


void Graph2dHybridWindowDataItem::buildData(
	const QVector<double>& xvals, const QVector<double>& yvals,
	const Graph2dHybridWindowResultSetting& s1,
	const Graph2dHybridWindowResultSetting::Setting& s2,
	QVector<double>& modxvals, QVector<double>& modyvals)
{
	QVector<double> indexvals;
	QVector<double> srcxvals;
	bool xIsIndex = false;
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = s1.targetDataTypeInfo();
	indexvals.reserve(xvals.size());
	if (info->gridLocation == Vertex || s1.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
		for (int i = 0; i < xvals.count(); ++i) {
			indexvals.append(i + 1);
		}
	} else if (info->gridLocation == CellCenter) {
#if STEPWISE
		int size = (xvals.size() + 2) / 2;
		for (int i = 0; i < size; ++i) {
			if (i == 0) {
				indexvals.append(i + 1);
			} else if (i == size - 1) {
				indexvals.append(i + 1);
			} else {
				indexvals.append(i + 1);
				indexvals.append(i + 1);
			}
		}
#else
		for (int i = 0; i < xvals.count(); ++i) {
			indexvals.append(i + 1);
		}
#endif
	} else {
		Q_ASSERT(false);   //   Unhandled GridLocation_t
	}
	Q_ASSERT(indexvals.size() == xvals.size());
	if (s1.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
		xIsIndex = s1.timeValueType() == Graph2dHybridWindowResultSetting::tvtTimeStep;
	} else {
		xIsIndex = s1.positionValueType() == Graph2dHybridWindowResultSetting::pvtIndex;
	}
	if (xIsIndex) {
		srcxvals = indexvals;
	} else {
		srcxvals = xvals;
	}
	if (s2.isBarChart()) {
		buildBarXYValues(srcxvals, yvals, modxvals, modyvals);
	} else {
		modxvals = srcxvals;
		modyvals = yvals;
	}
}

void Graph2dHybridWindowDataItem::buildXY(const QVector<double>& xvals, const QVector<double>& yvals, double** x, double** y)
{
	*x = new double[xvals.size()];
	*y = new double[yvals.size()];
	for (int i = 0; i < xvals.size(); ++i) {
		*(*x + i) = xvals.at(i);
		*(*y + i) = yvals.at(i);
	}
}

void Graph2dHybridWindowDataItem::buildBarXYValues(
	const QVector<double>& xvals, const QVector<double>& yvals,
	QVector<double>& modxvals, QVector<double>& modyvals)
{
	modxvals.reserve(xvals.size() * 3 + 1);
	modyvals.reserve(yvals.size() * 3 + 1);
	double firstx = 0;
	if (xvals.count() == 1) {
		firstx = xvals.at(0) - 1;
	} else if (xvals.count() > 1) {
		double firstwidth = xvals.at(1) - xvals.at(0);
		firstx = xvals.at(0) - firstwidth;
	}
	double xstart = firstx;
	modxvals.append(xstart);
	modyvals.append(0);

	for (int i = 0; i < xvals.size(); ++i) {
		double xend = xvals.at(i);
		double yval = yvals.at(i);
		double xdelta = 0;

		modxvals.append(xstart + xdelta);
		modyvals.append(yval);
		modxvals.append(xend - xdelta);
		modyvals.append(yval);
		modxvals.append(xend);
		modyvals.append(0);
		xstart = xend;
	}
}
