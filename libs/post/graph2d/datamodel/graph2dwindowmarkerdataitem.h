#ifndef GRAPH2DWINDOWMARKERDATAITEM_H
#define GRAPH2DWINDOWMARKERDATAITEM_H

#include "../graph2dwindowdataitem.h"
#include "../graph2dwindowmarkersetting.h"

class QColor;
class QwtPlotCustomMarker;

class Graph2dWindowMarkerDataItem : public Graph2dWindowDataItem
{

public:
	Graph2dWindowMarkerDataItem(const Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item, Graph2dWindowDataItem* parent);
	virtual ~Graph2dWindowMarkerDataItem();
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override {}
	QwtPlotCustomMarker* marker() const {return m_marker;}
	const Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& setting() const {return m_setting;}
	void update();

protected:
	void setVisible(bool visible) override;

private:
	bool m_attached;

	Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem m_setting;
	QwtPlotCustomMarker* m_marker;
};

#endif // GRAPH2DWINDOWMARKERDATAITEM_H
