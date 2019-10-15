#ifndef GRAPH2DVERIFICATIONWINDOWDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <post/graph2d/graph2dwindowdataitem.h>
#include "graph2dverificationwindowdatamodel.h"
#include <QList>

class Graph2dVerificationWindow;
class Graph2dVerificationWindowDataModel;
class PostSolutionInfo;

class Graph2dVerificationWindowDataItem : public Graph2dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Graph2dVerificationWindowDataItem(Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(parent)
	{}
	/// Constructor
	Graph2dVerificationWindowDataItem(const QString& itemlabel, Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(itemlabel, parent)
	{}
	/// Constructor
	Graph2dVerificationWindowDataItem(const QString& itemlabel, const QIcon& icon, Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(itemlabel, icon, parent)
	{}
	Graph2dVerificationWindow* graphWindow();

protected:
	PostSolutionInfo* postSolutionInfo();
	Graph2dVerificationWindowDataModel* dataModel();
#if SKIP
	static void buildData(const QVector<double>& xvals, const QVector<double>& yvals,
												const Graph2dVerificationWindowResultSetting& s1,
												const Graph2dVerificationWindowResultSetting::Setting& s2,
												QVector<double>& modxvals, QVector<double>& modyvals);
#endif
	static void buildXY(const QVector<double>& xvals, const QVector<double>& yvals, double** x, double** y);
};

#endif // GRAPH2DVERIFICATIONWINDOWDATAITEM_H
