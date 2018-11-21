#ifndef GRAPH2DHYBRIDWINDOWDATAITEM_H
#define GRAPH2DHYBRIDWINDOWDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <post/graph2d/graph2dwindowdataitem.h>
#include "graph2dhybridwindowdatamodel.h"
#include <QList>

#define STEPWISE 1

class Graph2dHybridWindow;
class Graph2dHybridWindowDataModel;
class PostSolutionInfo;

class Graph2dHybridWindowDataItem : public Graph2dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Graph2dHybridWindowDataItem(Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(parent)
	{}
	/// Constructor
	Graph2dHybridWindowDataItem(const QString& itemlabel, Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(itemlabel, parent)
	{}
	/// Constructor
	Graph2dHybridWindowDataItem(const QString& itemlabel, const QIcon& icon, Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(itemlabel, icon, parent)
	{}
	Graph2dHybridWindow* graphWindow();

protected:
	PostSolutionInfo* postSolutionInfo();
	Graph2dHybridWindowDataModel* dataModel();
	static void buildData(const QVector<double>& xvals, const QVector<double>& yvals,
												const Graph2dHybridWindowResultSetting& s1,
												const Graph2dHybridWindowResultSetting::Setting& s2,
												QVector<double>& modxvals, QVector<double>& modyvals);
	static void buildBarXYValues(const QVector<double>& xvals, const QVector<double>& yvals, QVector<double>& modxvals, QVector<double>& modyvals);
	static void buildXY(const QVector<double>& xvals, const QVector<double>& yvals, double** x, double** y);
};

#endif // GRAPH2DHYBRIDWINDOWDATAITEM_H
