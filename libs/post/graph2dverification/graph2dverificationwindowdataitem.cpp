#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowdataitem.h"
#include "graph2dverificationwindowdatamodel.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

Graph2dVerificationWindow* Graph2dVerificationWindowDataItem::graphWindow()
{
	return dynamic_cast<Graph2dVerificationWindow*>(dataModel()->mainWindow());
}

PostSolutionInfo* Graph2dVerificationWindowDataItem::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

Graph2dVerificationWindowDataModel* Graph2dVerificationWindowDataItem::dataModel()
{
	return dynamic_cast<Graph2dVerificationWindowDataModel*>(Graph2dWindowDataItem::dataModel());
}


#if SKIP
void Graph2dVerificationWindowDataItem::buildData(
	const QVector<double>& xvals, const QVector<double>& yvals,
	const Graph2dVerificationWindowResultSetting& s1,
	const Graph2dVerificationWindowResultSetting::Setting& s2,
	QVector<double>& modxvals, QVector<double>& modyvals)
{
	QVector<double> indexvals;
	QVector<double> srcxvals;
	bool xIsIndex = false;
	//Graph2dVerificationWindowResultSetting::DataTypeInfo* info = s1.targetDataTypeInfo();
	//indexvals.reserve(xvals.size());
	//if (info->gridLocation == Vertex || s1.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
	//	for (int i = 0; i < xvals.count(); ++i) {
	//		indexvals.append(i + 1);
	//	}
	//} else if (info->gridLocation == CellCenter) {
	//	for (int i = 0; i < xvals.count(); ++i) {
	//		indexvals.append(i + 1);
	//	}
	//} else {
	//	Q_ASSERT(false);   //   Unhandled GridLocation_t
	//}
	//Q_ASSERT(indexvals.size() == xvals.size());
	if (s1.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
		xIsIndex = s1.timeValueType() == Graph2dVerificationWindowResultSetting::tvtTimeStep;
	} else {
		xIsIndex = s1.positionValueType() == Graph2dVerificationWindowResultSetting::pvtIndex;
	}
	if (xIsIndex) {
		srcxvals = indexvals;
	} else {
		srcxvals = xvals;
	}
}
#endif

void Graph2dVerificationWindowDataItem::buildXY(const QVector<double>& xvals, const QVector<double>& yvals, double** x, double** y)
{
	*x = new double[xvals.size()];
	*y = new double[yvals.size()];
	for (int i = 0; i < xvals.size(); ++i) {
		*(*x + i) = xvals.at(i);
		*(*y + i) = yvals.at(i);
	}
}
