#include "../graph2dverificationwindow.h"
#include "graph2dverificationwindowrootdataitem.h"

Graph2dVerificationWindowRootDataItem::Graph2dVerificationWindowRootDataItem(Graph2dVerificationWindow* window, ProjectDataItem* parent) :
	Graph2dWindowRootDataItem(window, parent)
{
	updateItemMap();
}

Graph2dVerificationWindowRootDataItem::~Graph2dVerificationWindowRootDataItem()
{}

void Graph2dVerificationWindowRootDataItem::setupStandardModel(QStandardItemModel* /* model */)
{}

void Graph2dVerificationWindowRootDataItem::updateData()
{}
