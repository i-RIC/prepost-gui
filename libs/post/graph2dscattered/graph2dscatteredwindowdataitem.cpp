#include "graph2dscatteredwindow.h"
#include "graph2dscatteredwindowdataitem.h"
#include "graph2dscatteredwindowdatamodel.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

Graph2dScatteredWindow* Graph2dScatteredWindowDataItem::graphWindow()
{
	return dynamic_cast<Graph2dScatteredWindow*>(dataModel()->mainWindow());
}

PostSolutionInfo* Graph2dScatteredWindowDataItem::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

Graph2dScatteredWindowDataModel* Graph2dScatteredWindowDataItem::dataModel()
{
	return dynamic_cast<Graph2dScatteredWindowDataModel*>(Graph2dWindowDataItem::dataModel());
}

void Graph2dScatteredWindowDataItem::buildXY(const QVector<double>& xvals, const QVector<double>& yvals, double** x, double** y)
{
	*x = new double[xvals.size()];
	*y = new double[yvals.size()];
	for (int i = 0; i < xvals.size(); ++i) {
		*(*x + i) = xvals.at(i);
		*(*y + i) = yvals.at(i);
	}
}

