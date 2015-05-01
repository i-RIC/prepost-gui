#include "graph2dwindowmarkersetting.h"
#include "datamodel/graph2dwindowmarkerdataitem.h"
#include <misc/xmlsupport.h>
#include <qwt_plot.h>


#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dWindowMarkerSetting::Graph2dWindowMarkerSetting()
{
}

Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem::Graph2dWindowMarkerSettingItem()
{
	m_orientation = Graph2dWindowMarkerSetting::oHorizontal;
	m_value = 0;
	m_labelMode = Graph2dWindowMarkerSetting::lmAuto;

	m_penStyle = Qt::SolidLine;
	m_color = Qt::black;
	m_showLine = true;
	m_axis = QwtPlot::yLeft;
	m_alignCenter = false;
}

QList<Graph2dWindowDataItem*> Graph2dWindowMarkerSetting::setupItems(Graph2dWindowDataItem* parent)
{
	return setupItems(m_items, parent);
}

QList<Graph2dWindowDataItem*> Graph2dWindowMarkerSetting::setupItems(const QList<Graph2dWindowMarkerSettingItem>& settings, Graph2dWindowDataItem* parent)
{
	QList<Graph2dWindowDataItem*> ret;
	for (auto it = settings.begin(); it != settings.end(); ++it) {
		Graph2dWindowMarkerDataItem* item = new Graph2dWindowMarkerDataItem(*it, parent);
		ret.append(item);
	}
	return ret;
}

void Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_orientation = static_cast<Orientation>(iRIC::getIntAttribute(node, "orientation"));
	m_value = iRIC::getDoubleAttribute(node, "value");
	m_labelMode = static_cast<LabelMode>(iRIC::getIntAttribute(node, "labelMode"));
	m_customLabel = elem.attribute("customLabel");
	m_label = elem.attribute("label");
	m_penStyle = static_cast<Qt::PenStyle>(iRIC::getIntAttribute(node, "penStyle"));
	m_showLine = iRIC::getBooleanAttribute(node, "showLine");
	m_color = iRIC::getColorAttribute(node, "color");
	m_axis = iRIC::getIntAttribute(node, "axis");
	m_alignCenter = iRIC::getBooleanAttribute(node, "alignCenter");
}

void Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem::saveToProjectMainFile(QXmlStreamWriter& writer) const
{
	iRIC::setIntAttribute(writer, "orientation", static_cast<int>(m_orientation));
	iRIC::setDoubleAttribute(writer, "value", m_value);
	iRIC::setIntAttribute(writer, "labelMode", static_cast<int>(m_labelMode));
	writer.writeAttribute("customLabel", m_customLabel);
	writer.writeAttribute("label", m_label);
	iRIC::setIntAttribute(writer, "penStyle", static_cast<int>(m_penStyle));
	iRIC::setBooleanAttribute(writer, "showLine", m_showLine);
	iRIC::setColorAttribute(writer, "color", m_color);
	iRIC::setIntAttribute(writer, "axis", m_axis);
	iRIC::setBooleanAttribute(writer, "alignCenter", m_alignCenter);
}
