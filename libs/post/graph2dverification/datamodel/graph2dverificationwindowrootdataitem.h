#ifndef GRAPH2DVERIFICATIONWINDOWROOTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWROOTDATAITEM_H

#include <post/graph2d/graph2dwindowrootdataitem.h>

class Graph2dVerificationWindow;

class Graph2dVerificationWindowRootDataItem : public Graph2dWindowRootDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Graph2dVerificationWindowRootDataItem(Graph2dVerificationWindow* window, ProjectDataItem* parent);
	/// Destructor
	~Graph2dVerificationWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
	void updateData();

signals:
	void standardModelSetuped();

public:
	friend class Graph2dVerificationWindowProjectDataItem;
	friend class Graph2dVerificationWindowDataModel;
};

#endif // GRAPH2DVERIFICATIONWINDOWROOTDATAITEM_H
