#ifndef GRAPH2DWINDOWMARKERSETTING_H
#define GRAPH2DWINDOWMARKERSETTING_H

#include "graph2d_global.h"
#include <QList>
#include <QString>
#include <QColor>

class Graph2dWindowDataItem;
class Graph2dWindowMarkerDataItem;
class QDomNode;
class QXmlStreamWriter;

class GRAPH2D_EXPORT Graph2dWindowMarkerSetting
{

public:
	enum Orientation {
		oHorizontal,
		oVertical
	};
	enum LabelMode {
		lmAuto,
		lmCustom
	};
	class GRAPH2D_EXPORT Graph2dWindowMarkerSettingItem
	{

	public:
		Graph2dWindowMarkerSettingItem();
		Orientation orientation() const {return m_orientation;}
		void setOrientation(Orientation o) {m_orientation = o;}
		double value() const {return m_value;}
		void setValue(double v) {m_value = v;}
		LabelMode labelMode() const {return m_labelMode;}
		void setLabelMode(LabelMode lm) {m_labelMode = lm;}
		const QString& customLabel() const {return m_customLabel;}
		void setCustomLabel(const QString& cs) {m_customLabel = cs;}
		const QString& label() const {return m_label;}
		void setLabel(const QString& l) {m_label = l;}
		Qt::PenStyle penStyle() const {return m_penStyle;}
		void setPenStyle(Qt::PenStyle ps) {m_penStyle = ps;}
		const QColor& color() const {return m_color;}
		void setColor(const QColor& c) {m_color = c;}
		int axis() const {return m_axis;}
		void setAxis(int a) {m_axis = a;}
		bool showLine() const {return m_showLine;}
		void setShowLine(bool showline) {m_showLine = showline;}
		bool alignCenter() const {return m_alignCenter;}
		void setAlignCenter(bool aligncenter) {m_alignCenter = aligncenter;}
		void loadFromProjectMainFile(const QDomNode& node);
		void saveToProjectMainFile(QXmlStreamWriter& writer) const;

	private:
		Orientation m_orientation;
		double m_value;
		LabelMode m_labelMode;
		QString m_customLabel;
		QString m_label;
		Qt::PenStyle m_penStyle;
		bool m_showLine;
		QColor m_color;
		int m_axis;
		bool m_alignCenter;
	};
	Graph2dWindowMarkerSetting();
	QList<Graph2dWindowMarkerSettingItem>& items() {return m_items;}
	const QList<Graph2dWindowMarkerSettingItem>& items() const {return m_items;}
	QList<Graph2dWindowDataItem*> setupItems(Graph2dWindowDataItem* parent);
	static QList<Graph2dWindowDataItem*> setupItems(const QList<Graph2dWindowMarkerSettingItem>& settings, Graph2dWindowDataItem* parent);

private:
	QList<Graph2dWindowMarkerSettingItem> m_items;
};

#endif // GRAPH2DWINDOWMARKERSETTING_H
