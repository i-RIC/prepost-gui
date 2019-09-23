#ifndef GRAPH2DVERIFICATIONWINDOWROOTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWROOTDATAITEM_H

#include <post/graph2d/graph2dwindowrootdataitem.h>
#include <post/graph2d/graph2dwindowdatamodel.h>
#include "../graph2dverificationwindowresultsetting.h"
#include <QList>
#include <QMap>

class Graph2dVerificationWindowDataModel;
class QStandardItemModel;
class Graph2dVerificationWindow;
class Graph2dVerificationWindowResultGroupDataItem;
class Graph2dVerificationWindowResultCopyGroupDataItem;
class Graph2dVerificationWindowImportDataGroupDataItem;
class Graph2dWindowMarkerGroupDataItem;

class Graph2dVerificationWindowRootDataItem : public Graph2dWindowRootDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Graph2dVerificationWindowRootDataItem(Graph2dVerificationWindow* window, ProjectDataItem* parent);
	/// Destructor
	~Graph2dVerificationWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
#if 0
	Graph2dVerificationWindowResultGroupDataItem* resultGroupItem() {
		return m_resultGroupItem;
	}
	Graph2dVerificationWindowResultCopyGroupDataItem* resultCopyGroupItem() {
		return m_resultCopyGroupItem;
	}
	Graph2dVerificationWindowImportDataGroupDataItem* importDataGroupItem() {
		return m_importDataGroupItem;
	}
	Graph2dWindowMarkerGroupDataItem* markerGroupItem() {
		return m_markerGroupItem;
	}
#endif
	void updateData(int fn);
	bool axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const;

signals:
	void standardModelSetuped();

protected:
#if 0
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
#endif

private:
	//Graph2dVerificationWindowResultGroupDataItem* m_resultGroupItem;
	//Graph2dVerificationWindowResultCopyGroupDataItem* m_resultCopyGroupItem;
	//Graph2dVerificationWindowImportDataGroupDataItem* m_importDataGroupItem;
	//Graph2dWindowMarkerGroupDataItem* m_markerGroupItem;

public:
	friend class Graph2dVerificationWindowProjectDataItem;
	friend class Graph2dVerificationWindowDataModel;
};

#endif // GRAPH2DVERIFICATIONWINDOWROOTDATAITEM_H
