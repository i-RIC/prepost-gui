#include "graph2dhybridwindow.h"
#include "graph2dhybridwindowdataitem.h"
#include "graph2dhybridwindowdatamodel.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

Graph2dHybridWindowDataItem::Graph2dHybridWindowDataItem(Graph2dWindowDataItem* parent) :
	Graph2dWindowDataItem(parent)
{}

Graph2dHybridWindowDataItem::Graph2dHybridWindowDataItem(const QString& itemlabel, Graph2dWindowDataItem* parent) :
	Graph2dWindowDataItem(itemlabel, parent)
{}

Graph2dHybridWindowDataItem::Graph2dHybridWindowDataItem(const QString& itemlabel, const QIcon& icon, Graph2dWindowDataItem* parent) :
	Graph2dWindowDataItem(itemlabel, icon, parent)
{}

Graph2dHybridWindow* Graph2dHybridWindowDataItem::graphWindow()
{
	return dynamic_cast<Graph2dHybridWindow*>(dataModel()->mainWindow());
}

PostSolutionInfo* Graph2dHybridWindowDataItem::postSolutionInfo() const
{
	return projectData()->mainfile()->postSolutionInfo();
}

Graph2dHybridWindowDataModel* Graph2dHybridWindowDataItem::dataModel()
{
	return dynamic_cast<Graph2dHybridWindowDataModel*>(Graph2dWindowDataItem::dataModel());
}

void Graph2dHybridWindowDataItem::buildData(
	const std::vector<double>& xvals, const std::vector<double>& yvals,
	const Graph2dHybridWindowResultSetting& s1,
	const Graph2dHybridWindowResultSetting::Setting& s2,
	std::vector<double>* modxvals, std::vector<double>* modyvals)
{
	std::vector<double> indexvals;
	std::vector<double> srcxvals;
	bool xIsIndex = false;

	if (s1.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
		xIsIndex = s1.timeValueType() == Graph2dHybridWindowResultSetting::tvtTimeStep;
	} else {
		xIsIndex = s1.positionValueType() == Graph2dHybridWindowResultSetting::pvtIndex;
	}
	if (xIsIndex) {
		indexvals.reserve(xvals.size());
		for (unsigned int i = 0; i < xvals.size(); ++i) {
			indexvals.push_back(i + 1);
		}
		srcxvals = indexvals;
	} else {
		srcxvals = xvals;
		if (s1.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			double scale = 1.0;
			auto tu = s1.xAxisTimeUnit();
			if (tu == Graph2dHybridWindowResultSetting::tuMinutes) {
				scale = 1.0 / 60;
			} else if (tu == Graph2dHybridWindowResultSetting::tuHours) {
				scale = 1.0 / (60 * 60);
			} else if (tu == Graph2dHybridWindowResultSetting::tuDays) {
				scale = 1.0 / (60 * 60 * 24);
			}
			for (int i = 0; i < srcxvals.size(); ++i) {
				srcxvals[i] *= scale;
			}
		}
	}

	if (s2.isBarChart()) {
		buildBarXYValues(srcxvals, yvals, modxvals, modyvals);
	} else {
		*modxvals = srcxvals;
		*modyvals = yvals;
	}
}

void Graph2dHybridWindowDataItem::buildBarXYValues(
	const std::vector<double>& xvals, const std::vector<double>& yvals,
	std::vector<double>* modxvals, std::vector<double>* modyvals)
{
	modxvals->assign(xvals.size() * 3 + 1, 0);
	modyvals->assign(yvals.size() * 3 + 1, 0);
	double firstx = 0;
	if (xvals.size() == 1) {
		firstx = xvals.at(0) - 1;
	} else if (xvals.size() > 1) {
		double firstwidth = xvals.at(1) - xvals.at(0);
		firstx = xvals.at(0) - firstwidth;
	}
	double xstart = firstx;
	modxvals->push_back(xstart);
	modyvals->push_back(0);

	for (unsigned int i = 0; i < xvals.size(); ++i) {
		double xend = xvals.at(i);
		double yval = yvals.at(i);
		double xdelta = 0;

		modxvals->push_back(xstart + xdelta);
		modyvals->push_back(yval);
		modxvals->push_back(xend - xdelta);
		modyvals->push_back(yval);
		modxvals->push_back(xend);
		modyvals->push_back(0);
		xstart = xend;
	}
}
