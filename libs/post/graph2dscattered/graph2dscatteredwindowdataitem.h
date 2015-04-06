#ifndef GRAPH2DSCATTEREDWINDOWDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <post/graph2d/graph2dwindowdataitem.h>
#include "graph2dscatteredwindowdatamodel.h"
#include <QList>

class Graph2dScatteredWindow;
class Graph2dScatteredWindowDataModel;
class PostSolutionInfo;

class Graph2dScatteredWindowDataItem : public Graph2dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Graph2dScatteredWindowDataItem(Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(parent)
	{}
	/// Constructor
	Graph2dScatteredWindowDataItem(const QString& itemlabel, Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(itemlabel, parent)
	{}
	/// Constructor
	Graph2dScatteredWindowDataItem(const QString& itemlabel, const QIcon& icon, Graph2dWindowDataItem* parent)
		: Graph2dWindowDataItem(itemlabel, icon, parent)
	{}
	Graph2dScatteredWindow* graphWindow();

protected:
	void buildXY(const QVector<double>& xvals, const QVector<double>& yvals, double** x, double** y);

	PostSolutionInfo* postSolutionInfo();
	Graph2dScatteredWindowDataModel* dataModel();
};

#endif // GRAPH2DSCATTEREDWINDOWDATAITEM_H
