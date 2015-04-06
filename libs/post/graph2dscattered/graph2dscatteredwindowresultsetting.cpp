#include "graph2dscatteredwindowresultsetting.h"
#include <postbase/postallzoneselectingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guibase/qwtplotcustomcurve.h>
#include <misc/xmlsupport.h>
#include "datamodel/graph2dscatteredwindowresultgroupdataitem.h"
#include "datamodel/graph2dscatteredwindowgridresultdataitem.h"
#include <guicore/project/colorsource.h>

#include <qwt_plot.h>

const QString Graph2dScatteredWindowResultSetting::XAXIS_POSITION_X = "_positionX";
const QString Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Y = "_positionY";
const QString Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Z = "_positionZ";
const QString Graph2dScatteredWindowResultSetting::XAXIS_STREAM_WISE_DISTANCE = "_streamWiseDistance";

ColorSource* Graph2dScatteredWindowResultSetting::m_colorSource = new ColorSource(0);

Graph2dScatteredWindowResultSetting::Graph2dScatteredWindowResultSetting()
{
	m_xAxisReverse = false;
	m_xAxisLog = false;

	m_yAxisLeftAutoRange = true;
	m_yAxisLeftTitle = "";
	m_yAxisLeftReverse = false;
	m_yAxisLeftLog = false;
	m_yAxisRightAutoRange = true;
	m_yAxisRightTitle = "";
	m_yAxisRightReverse = false;
	m_yAxisRightLog = false;

	m_xAxisAutoRange = true;
	m_xAxisValueMin = 0;
	m_xAxisValueMax = 0;

	m_addTimeToTitle = false;
}

Graph2dScatteredWindowResultSetting::~Graph2dScatteredWindowResultSetting()
{

}

bool Graph2dScatteredWindowResultSetting::init(PostSolutionInfo* sol)
{
	PostAllZoneSelectingDialog dialog(0);
	bool ok = dialog.setPostSolutionInfo(sol);
	// if setup fail, return false;
	if (! ok){return false;}

	if (dialog.needToSelect()){
		int result = dialog.exec();
		if (result == QDialog::Rejected){return false;}
	}
	m_dimension = dialog.dimension();
	PostZoneDataContainer* cont = dialog.container();
	m_zoneId = cont->zoneId();
	m_zoneName = cont->zoneName();
	return true;
}

void Graph2dScatteredWindowResultSetting::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_xAxis = elem.attribute("xAxis");
	m_xAxisAutoRange = iRIC::getBooleanAttribute(node, "xAxisAutoRange");
	m_xAxisValueMin = iRIC::getDoubleAttribute(node, "xAxisValueMin");
	m_xAxisValueMax = iRIC::getDoubleAttribute(node, "xAxisValueMax");
	m_xAxisReverse = iRIC::getBooleanAttribute(node, "xAxisReverse");
	m_xAxisLog = iRIC::getBooleanAttribute(node, "xAxisLog");
	m_xAxisLabel = elem.attribute("xAxisLabel");

	m_yAxisLeftAutoRange = iRIC::getBooleanAttribute(node, "yAxisLeftAutoRange");
	m_yAxisLeftMin = iRIC::getDoubleAttribute(node, "yAxisLeftMin");
	m_yAxisLeftMax = iRIC::getDoubleAttribute(node, "yAxisLeftMax");
	m_yAxisLeftReverse = iRIC::getBooleanAttribute(node, "yAxisLeftReverse");
	m_yAxisLeftLog = iRIC::getBooleanAttribute(node, "yAxisLeftLog");
	m_yAxisLeftTitle = elem.attribute("yAxisLeftTitle");

	m_yAxisRightAutoRange = iRIC::getBooleanAttribute(node, "yAxisRightAutoRange");
	m_yAxisRightMin = iRIC::getDoubleAttribute(node, "yAxisRightMin");
	m_yAxisRightMax = iRIC::getDoubleAttribute(node, "yAxisRightMax");
	m_yAxisRightReverse = iRIC::getBooleanAttribute(node, "yAxisRightReverse");
	m_yAxisRightLog = iRIC::getBooleanAttribute(node, "yAxisRightLog");
	m_yAxisRightTitle = elem.attribute("yAxisRightTitle");

	m_title = elem.attribute("title");
	m_addTimeToTitle = iRIC::getBooleanAttribute(node, "addTimeToTitle");

	m_targetDatas.clear();
	QDomNode datasNode = iRIC::getChildNode(node, "TargetDatas");
	if (! datasNode.isNull()){
		for (int i = 0; i < datasNode.childNodes().count(); ++i){
			Graph2dScatteredWindowResultSetting::Setting setting;
			setting.loadFromProjectMainFile(datasNode.childNodes().at(i));
			m_targetDatas.append(setting);
		}
	}
}

void Graph2dScatteredWindowResultSetting::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("xAxis", m_xAxis);
	iRIC::setBooleanAttribute(writer, "xAxisAutoRange", m_xAxisAutoRange);
	iRIC::setDoubleAttribute(writer, "xAxisValueMin", m_xAxisValueMin);
	iRIC::setDoubleAttribute(writer, "xAxisValueMax", m_xAxisValueMax);
	iRIC::setBooleanAttribute(writer, "xAxisReverse", m_xAxisReverse);
	iRIC::setBooleanAttribute(writer, "xAxisLog", m_xAxisLog);
	writer.writeAttribute("xAxisLabel", m_xAxisLabel);

	iRIC::setBooleanAttribute(writer, "yAxisLeftAutoRange", m_yAxisLeftAutoRange);
	iRIC::setDoubleAttribute(writer, "yAxisLeftMin", m_yAxisLeftMin);
	iRIC::setDoubleAttribute(writer, "yAxisLeftMax", m_yAxisLeftMax);
	iRIC::setBooleanAttribute(writer, "yAxisLeftReverse", m_yAxisLeftReverse);
	iRIC::setBooleanAttribute(writer, "yAxisLeftLog", m_yAxisLeftLog);
	writer.writeAttribute("yAxisLeftTitle", m_yAxisLeftTitle);

	iRIC::setBooleanAttribute(writer, "yAxisRightAutoRange", m_yAxisRightAutoRange);
	iRIC::setDoubleAttribute(writer, "yAxisRightMin", m_yAxisRightMin);
	iRIC::setDoubleAttribute(writer, "yAxisRightMax", m_yAxisRightMax);
	iRIC::setBooleanAttribute(writer, "yAxisRightReverse", m_yAxisRightReverse);
	iRIC::setBooleanAttribute(writer, "yAxisRightLog", m_yAxisRightLog);
	writer.writeAttribute("yAxisRightTitle", m_yAxisRightTitle);

	writer.writeAttribute("title", m_title);
	iRIC::setBooleanAttribute(writer, "addTimeToTitle", m_addTimeToTitle);

	// targetDatas
	writer.writeStartElement("TargetDatas");
	for (int i = 0; i < m_targetDatas.count(); ++i){
		writer.writeStartElement("TargetData");
		const Graph2dScatteredWindowResultSetting::Setting& setting = m_targetDatas.at(i);
		setting.saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void Graph2dScatteredWindowResultSetting::Setting::loadFromProjectMainFile(const QDomNode &node)
{
	QDomElement elem = node.toElement();

	m_name = elem.attribute("name");
	m_axisSide = static_cast<AxisSide>(iRIC::getIntAttribute(node, "axisSide"));
	m_customColor = iRIC::getColorAttribute(node, "customColor");
	m_symbolType = static_cast<SymbolType>(iRIC::getIntAttribute(node, "symbolType"));
	m_symbolSize = iRIC::getIntAttribute(node, "symbolSize");
}

void Graph2dScatteredWindowResultSetting::Setting::saveToProjectMainFile(QXmlStreamWriter &writer) const
{
	writer.writeAttribute("name", m_name);
	iRIC::setIntAttribute(writer, "axisSide", static_cast<int>(m_axisSide));
	iRIC::setColorAttribute(writer, "customColor", m_customColor);
	iRIC::setIntAttribute(writer, "symbolType", static_cast<int>(m_symbolType));
	iRIC::setIntAttribute(writer, "symbolSize", m_symbolSize);
}

void Graph2dScatteredWindowResultSetting::Setting::setupCurve(QwtPlotCustomCurve *curve) const
{
	if (m_axisSide == Graph2dScatteredWindowResultSetting::asLeft){
		curve->setYAxis(QwtPlot::yLeft);
	} else {
		curve->setYAxis(QwtPlot::yRight);
	}

	// symbol
	// no line.
	QPen pen;
	pen.setColor(Qt::transparent);
	pen.setWidth(1);
	pen.setStyle(Qt::SolidLine);
	curve->setPen(pen);
	QwtSymbol* symbol = new QwtSymbol();
	symbol->setStyle(Graph2dScatteredWindowResultSetting::getSymbolStyle(m_symbolType));
	symbol->setSize(m_symbolSize);
	pen.setColor(m_customColor);
	pen.setWidth(1);
	pen.setStyle(Qt::SolidLine);
	symbol->setPen(pen);
	QBrush brush;
	brush.setColor(Qt::transparent);
	symbol->setBrush(brush);
	curve->setSymbol(symbol);
}

QwtSymbol::Style Graph2dScatteredWindowResultSetting::getSymbolStyle(SymbolType st)
{
	switch (st){
	case symCircle:
		return QwtSymbol::Ellipse;
		break;
	case symRect:
		return QwtSymbol::Rect;
		break;
	case symDiamond:
		return QwtSymbol::Diamond;
		break;
	case symTriangle:
		return QwtSymbol::Triangle;
		break;
	case symCross:
		return QwtSymbol::Cross;
		break;
	case symXCross:
		return QwtSymbol::XCross;
		break;
	default:
		return QwtSymbol::Ellipse;
	}
}

const QString Graph2dScatteredWindowResultSetting::autoYAxisLabel(AxisSide as){
	QStringList labels;
	for (int i = 0; i < m_targetDatas.count(); ++i){
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as){
			labels.append(s.name());
		}
	}
	return labels.join(", ");
}

bool Graph2dScatteredWindowResultSetting::axisNeeded(AxisSide as)
{
	for (int i = 0; i < m_targetDatas.count(); ++i){
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as){return true;}
	}
	return false;
}

QList<Graph2dWindowDataItem*> Graph2dScatteredWindowResultSetting::setupItems(Graph2dScatteredWindowResultGroupDataItem* gItem) const
{
	QList<Graph2dWindowDataItem*> ret;
	for (int i = 0; i < m_targetDatas.count(); ++i){
		Graph2dScatteredWindowGridResultDataItem* item = new Graph2dScatteredWindowGridResultDataItem(m_targetDatas[i], i, gItem);
		ret.append(item);
	}
	return ret;
}

void Graph2dScatteredWindowResultSetting::setAutoXAxisLabel()
{
	QString xAxisLabel;
	if (m_xAxis == XAXIS_POSITION_X){
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Position X");
	} else if (m_xAxis == XAXIS_POSITION_Y){
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Position Y");
	} else if (m_xAxis == XAXIS_POSITION_Z){
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Position Z");
	} else if (m_xAxis == XAXIS_STREAM_WISE_DISTANCE){
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Stream-wise Distance");
	} else {
		m_xAxisLabel = m_xAxis;
	}
}

QColor Graph2dScatteredWindowResultSetting::autoColor(int index)
{
	return m_colorSource->getColor(index);
}
