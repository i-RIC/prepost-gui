#ifndef GRAPH2DSCATTEREDWINDOWROOTDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWROOTDATAITEM_H

#include <post/graph2d/graph2dwindowrootdataitem.h>
#include <post/graph2d/graph2dwindowdatamodel.h>
#include "../graph2dscatteredwindowresultsetting.h"
#include <QList>
#include <QMap>

class Graph2dScatteredWindowDataModel;
class QStandardItemModel;
class Graph2dScatteredWindow;
class Graph2dScatteredWindowResultGroupDataItem;

class Graph2dScatteredWindowRootDataItem : public Graph2dWindowRootDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Graph2dScatteredWindowRootDataItem(Graph2dScatteredWindow* window, ProjectDataItem* parent);
	/// Destructor
	~Graph2dScatteredWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
	Graph2dScatteredWindowResultGroupDataItem* resultGroupItem(){
		return m_resultGroupItem;
	}
	void updateData(int fn);
	bool axisNeeded(Graph2dScatteredWindowResultSetting::AxisSide as) const;

signals:
	void standardModelSetuped();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	Graph2dScatteredWindowResultGroupDataItem* m_resultGroupItem;

public:
	friend class Graph2dScatteredWindowProjectDataItem;
	friend class Graph2dScatteredWindowDataModel;
};

#endif // GRAPH2DSCATTEREDWINDOWROOTDATAITEM_H
