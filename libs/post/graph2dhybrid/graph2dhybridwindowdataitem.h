#ifndef GRAPH2DHYBRIDWINDOWDATAITEM_H
#define GRAPH2DHYBRIDWINDOWDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <post/graph2d/graph2dwindowdataitem.h>
#include "graph2dhybridwindowdatamodel.h"

#include <vector>

class Graph2dHybridWindow;
class Graph2dHybridWindowDataModel;
class PostSolutionInfo;

class Graph2dHybridWindowDataItem : public Graph2dWindowDataItem
{
	Q_OBJECT

public:
	Graph2dHybridWindowDataItem(Graph2dWindowDataItem* parent);
	Graph2dHybridWindowDataItem(const QString& itemlabel, Graph2dWindowDataItem* parent);
	Graph2dHybridWindowDataItem(const QString& itemlabel, const QIcon& icon, Graph2dWindowDataItem* parent);

	Graph2dHybridWindow* graphWindow();

protected:
	PostSolutionInfo* postSolutionInfo() const;
	Graph2dHybridWindowDataModel* dataModel();

	static void buildData(const std::vector<double>& xvals, const std::vector<double>& yvals,
												const Graph2dHybridWindowResultSetting& s1,
												const Graph2dHybridWindowResultSetting::Setting& s2,
												std::vector<double>* modxvals, std::vector<double>* modyvals);
	static void buildBarXYValues(const std::vector<double>& xvals, const std::vector<double>& yvals, std::vector<double>* modxvals, std::vector<double>* modyvals);
};

#endif // GRAPH2DHYBRIDWINDOWDATAITEM_H
