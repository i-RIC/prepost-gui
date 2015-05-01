#ifndef GRAPH2DWINDOWMARKERGROUPDATAITEM_H
#define GRAPH2DWINDOWMARKERGROUPDATAITEM_H

#include "../graph2d_global.h"
#include "../graph2dwindowdataitem.h"
#include "../graph2dwindowmarkersetting.h"
#include "../graph2dwindowdatamodel.h"
#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class Post2dWindowZoneDataItem;
class LookupTableContainer;
class Graph2dPositionWindowHorizontalAxisSetting;
class ColorSource;

class GRAPH2D_EXPORT Graph2dWindowMarkerGroupDataItem : public Graph2dWindowDataItem
{
	Q_OBJECT
public:
	Graph2dWindowMarkerGroupDataItem(Graph2dWindowDataItem* parent);
	~Graph2dWindowMarkerGroupDataItem();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	const Graph2dWindowMarkerSetting& setting() {return m_setting;}
	void setXLabel(const QString& l) {m_xLabel = l;}
	void addMarkers(const QList<Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem>& settings);
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	Graph2dWindowMarkerSetting m_setting;
	QString m_xLabel;
	ColorSource* m_colorSource;
};

#endif // GRAPH2DWINDOWMARKERGROUPDATAITEM_H
