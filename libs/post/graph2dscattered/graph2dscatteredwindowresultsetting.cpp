#include "datamodel/graph2dscatteredwindowgridresultdataitem.h"
#include "datamodel/graph2dscatteredwindowresultgroupdataitem.h"
#include "graph2dscatteredwindowresultsetting.h"

#include <guibase/qwtplotcustomcurve.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/colorsource.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <postbase/postallzoneselectingdialog.h>
#include <misc/stringtool.h>

#include <qwt_plot.h>

#include <QDomNode>
#include <QSettings>

const std::string Graph2dScatteredWindowResultSetting::XAXIS_POSITION_X = "_positionX";
const std::string Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Y = "_positionY";
const std::string Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Z = "_positionZ";
const std::string Graph2dScatteredWindowResultSetting::XAXIS_STREAM_WISE_DISTANCE = "_streamWiseDistance";

ColorSource* Graph2dScatteredWindowResultSetting::m_colorSource = new ColorSource(0);



Graph2dScatteredWindowResultSetting::Setting::Setting() :
	m_name {""},
	m_axisSide {AxisSide::asLeft},
	m_customColor {Qt::black},
	m_symbolType {SymbolType::symCircle},
	m_symbolSize {7}
{}

Graph2dScatteredWindowResultSetting::Setting::Setting(const std::string& name) :
	m_name {name},
	m_axisSide {AxisSide::asLeft},
	m_customColor {Qt::black},
	m_symbolType {SymbolType::symCircle},
	m_symbolSize {7}
{}

void Graph2dScatteredWindowResultSetting::Setting::setupCurve(QwtPlotCustomCurve* curve) const
{
	if (m_axisSide == Graph2dScatteredWindowResultSetting::asLeft) {
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

const std::string& Graph2dScatteredWindowResultSetting::Setting::name() const
{
	return m_name;
}

void Graph2dScatteredWindowResultSetting::Setting::setName(const std::string& name)
{
	m_name = name;
}

const QString& Graph2dScatteredWindowResultSetting::Setting::caption() const
{
	return m_caption;
}

void Graph2dScatteredWindowResultSetting::Setting::setCaption(const QString& caption)
{
	m_caption = caption;
}

Graph2dScatteredWindowResultSetting::AxisSide Graph2dScatteredWindowResultSetting::Setting::axisSide() const
{
	return m_axisSide;
}

void Graph2dScatteredWindowResultSetting::Setting::setAxisSide(AxisSide as)
{
	m_axisSide = as;
}

const QColor& Graph2dScatteredWindowResultSetting::Setting::customColor() const
{
	return m_customColor;
}

void Graph2dScatteredWindowResultSetting::Setting::setCustomColor(const QColor& c)
{
	m_customColor = c;
}

Graph2dScatteredWindowResultSetting::SymbolType Graph2dScatteredWindowResultSetting::Setting::symbolType() const{
	return m_symbolType;
}

void Graph2dScatteredWindowResultSetting::Setting::setSymbolType(SymbolType st)
{
	m_symbolType = st;
}

int Graph2dScatteredWindowResultSetting::Setting::symbolSize() const
{
	return m_symbolSize;
}

void Graph2dScatteredWindowResultSetting::Setting::setSymbolSize(int size)
{
	m_symbolSize = size;
}

void Graph2dScatteredWindowResultSetting::Setting::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();

	m_name = iRIC::toStr(elem.attribute("name"));
	m_caption = elem.attribute("caption");
	m_axisSide = static_cast<AxisSide>(iRIC::getIntAttribute(node, "axisSide"));
	m_customColor = iRIC::getColorAttribute(node, "customColor");
	m_symbolType = static_cast<SymbolType>(iRIC::getIntAttribute(node, "symbolType"));
	m_symbolSize = iRIC::getIntAttribute(node, "symbolSize");
}

void Graph2dScatteredWindowResultSetting::Setting::saveToProjectMainFile(QXmlStreamWriter& writer) const
{
	writer.writeAttribute("name", m_name.c_str());
	writer.writeAttribute("caption", m_caption);
	iRIC::setIntAttribute(writer, "axisSide", static_cast<int>(m_axisSide));
	iRIC::setColorAttribute(writer, "customColor", m_customColor);
	iRIC::setIntAttribute(writer, "symbolType", static_cast<int>(m_symbolType));
	iRIC::setIntAttribute(writer, "symbolSize", m_symbolSize);
}

Graph2dScatteredWindowResultSetting::Graph2dScatteredWindowResultSetting() :
	m_xAxisAutoRange {true},
	m_xAxisValueMin {0},
	m_xAxisValueMax {0},
	m_xAxisReverse {false},
	m_xAxisLog {false},
	m_yAxisLeftAutoRange {true},
	m_yAxisLeftReverse {false},
	m_yAxisLeftLog {false},
	m_yAxisLeftTitle {""},
	m_yAxisRightAutoRange {true},
	m_yAxisRightReverse {false},
	m_yAxisRightLog {false},
	m_yAxisRightTitle {""},
	m_addTimeToTitle {false},
	m_chartTitleFont {"chartTitle"},
	m_legendFont {"legend"},
	m_xAxisTitleFont {"xAxisTitle"},
	m_xAxisTickFont {"xAxisTick"},
	m_yAxisTitleFont {"yAxisTitle"},
	m_yAxisTickFont {"yAxisTick"}
{
	QSettings settings;
	QFont font;

	font.fromString(settings.value("fontsetting/chart_title", QString("MS UI Gothic,12,-1,5,75,0,0,0,0,0")).toString());
	m_chartTitleFont = font;

	font.fromString(settings.value("fontsetting/chart_legend", QString("MS UI Gothic,9,-1,5,50,0,0,0,0,0")).toString());
	m_legendFont = font;

	font.fromString(settings.value("fontsetting/chart_axistitle", QString("MS UI Gothic,12,-1,5,75,0,0,0,0,0")).toString());
	m_xAxisTitleFont = font;
	m_yAxisTitleFont = font;

	font.fromString(settings.value("fontsetting/chart_axistick", QString("MS UI Gothic,10,-1,5,50,0,0,0,0,0")).toString());
	m_xAxisTickFont = font;
	m_yAxisTickFont = font;
}

Graph2dScatteredWindowResultSetting::~Graph2dScatteredWindowResultSetting()
{}

bool Graph2dScatteredWindowResultSetting::init(PostSolutionInfo* sol)
{
	PostAllZoneSelectingDialog dialog(0);
	bool ok = dialog.setPostSolutionInfo(sol);
	// if setup fail, return false;
	if (! ok) {return false;}

	if (dialog.needToSelect()) {
		int result = dialog.exec();
		if (result == QDialog::Rejected) {return false;}
	}
	m_dimension = dialog.dimension();
	PostZoneDataContainer* cont = dialog.container();
	m_zoneName = cont->zoneName();
	return true;
}

PostSolutionInfo::Dimension Graph2dScatteredWindowResultSetting::dimension() const
{
	return m_dimension;
}

const std::string& Graph2dScatteredWindowResultSetting::zoneName() const
{
	return m_zoneName;
}

QFont Graph2dScatteredWindowResultSetting::chartTitleFont() const
{
	return m_chartTitleFont;
}

void Graph2dScatteredWindowResultSetting::setChartTitleFont(const QFont& font)
{
	m_chartTitleFont = font;
}

QFont Graph2dScatteredWindowResultSetting::legendFont() const
{
	return m_legendFont;
}

void Graph2dScatteredWindowResultSetting::setLegendFont(const QFont& font)
{
	m_legendFont = font;
}

QFont Graph2dScatteredWindowResultSetting::xAxisTitleFont() const
{
	return m_xAxisTitleFont;
}

void Graph2dScatteredWindowResultSetting::setXAxisTitleFont(const QFont& font)
{
	m_xAxisTitleFont = font;
}

QFont Graph2dScatteredWindowResultSetting::xAxisTickFont() const
{
	return m_xAxisTickFont;
}

void Graph2dScatteredWindowResultSetting::setXAxisTickFont(const QFont& font)
{
	m_xAxisTickFont = font;
}

QFont Graph2dScatteredWindowResultSetting::yAxisTitleFont() const
{
	return m_yAxisTitleFont;
}

void Graph2dScatteredWindowResultSetting::setYAxisTitleFont(const QFont& font)
{
	m_yAxisTitleFont = font;
}

QFont Graph2dScatteredWindowResultSetting::yAxisTickFont() const
{
	return m_yAxisTickFont;
}

void Graph2dScatteredWindowResultSetting::setYAxisTickFont(const QFont& font)
{
	m_yAxisTickFont = font;
}


void Graph2dScatteredWindowResultSetting::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_xAxis = iRIC::toStr(elem.attribute("xAxis"));
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

	m_chartTitleFont.load(node);
	m_legendFont.load(node);
	m_xAxisTitleFont.load(node);
	m_xAxisTickFont.load(node);
	m_yAxisTitleFont.load(node);
	m_yAxisTickFont.load(node);

	m_targetDatas.clear();
	QDomNode datasNode = iRIC::getChildNode(node, "TargetDatas");
	if (! datasNode.isNull()) {
		for (int i = 0; i < datasNode.childNodes().count(); ++i) {
			Graph2dScatteredWindowResultSetting::Setting setting;
			setting.loadFromProjectMainFile(datasNode.childNodes().at(i));
			m_targetDatas.append(setting);
		}
	}
}

void Graph2dScatteredWindowResultSetting::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("xAxis", m_xAxis.c_str());
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

	m_chartTitleFont.save(writer);
	m_legendFont.save(writer);
	m_xAxisTitleFont.save(writer);
	m_xAxisTickFont.save(writer);
	m_yAxisTitleFont.save(writer);
	m_yAxisTickFont.save(writer);

	// targetDatas
	writer.writeStartElement("TargetDatas");
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		writer.writeStartElement("TargetData");
		const Graph2dScatteredWindowResultSetting::Setting& setting = m_targetDatas.at(i);
		setting.saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

QwtSymbol::Style Graph2dScatteredWindowResultSetting::getSymbolStyle(SymbolType st)
{
	switch (st) {
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

QString Graph2dScatteredWindowResultSetting::autoYAxisLabel(AxisSide as)
{
	QStringList labels;
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as) {
			labels.append(s.caption());
		}
	}
	return labels.join(", ");
}

bool Graph2dScatteredWindowResultSetting::axisNeeded(AxisSide as)
{
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as) {return true;}
	}
	return false;
}

QList<Graph2dScatteredWindowResultSetting::Setting>& Graph2dScatteredWindowResultSetting::targetDatas()
{
	return m_targetDatas;
}

const QList<Graph2dScatteredWindowResultSetting::Setting>& Graph2dScatteredWindowResultSetting::targetDatas() const
{
	return m_targetDatas;
}

bool Graph2dScatteredWindowResultSetting::xAxisAutoRange() const
{
	return m_xAxisAutoRange;
}

void Graph2dScatteredWindowResultSetting::setXAxisAutoRange(bool a)
{
	m_xAxisAutoRange = a;
}

double Graph2dScatteredWindowResultSetting::xAxisValueMin() const
{
	return m_xAxisValueMin;
}

void Graph2dScatteredWindowResultSetting::setXAxisValueMin(double min)
{
	m_xAxisValueMin = min;
}

double Graph2dScatteredWindowResultSetting::xAxisValueMax() const
{
	return m_xAxisValueMax;
}

void Graph2dScatteredWindowResultSetting::setXAxisValueMax(double max)
{
	m_xAxisValueMax = max;
}

bool Graph2dScatteredWindowResultSetting::xAxisReverse() const
{
	return m_xAxisReverse;
}

void Graph2dScatteredWindowResultSetting::setXAxisReverse(bool rev)
{
	m_xAxisReverse = rev;
}

bool Graph2dScatteredWindowResultSetting::xAxisLog() const
{
	return m_xAxisLog;
}

void Graph2dScatteredWindowResultSetting::setXAxisLog(bool log)
{
	m_xAxisLog = log;
}

const QString& Graph2dScatteredWindowResultSetting::xAxisLabel() const
{
	return m_xAxisLabel;
}

void Graph2dScatteredWindowResultSetting::setXAxisLabel(const QString& l)
{
	m_xAxisLabel = l;
}

bool Graph2dScatteredWindowResultSetting::yAxisLeftAutoRange() const
{
	return m_yAxisLeftAutoRange;
}

void Graph2dScatteredWindowResultSetting::setYAxisLeftAutoRange(bool a)
{
	m_yAxisLeftAutoRange = a;
}

double Graph2dScatteredWindowResultSetting::yAxisLeftMin() const
{
	return m_yAxisLeftMin;
}

void Graph2dScatteredWindowResultSetting::setYAxisLeftMin(double min)
{
	m_yAxisLeftMin = min;
}

double Graph2dScatteredWindowResultSetting::yAxisLeftMax() const
{
	return m_yAxisLeftMax;
}

void Graph2dScatteredWindowResultSetting::setYAxisLeftMax(double max)
{
	m_yAxisLeftMax = max;
}

bool Graph2dScatteredWindowResultSetting::yAxisLeftReverse() const
{
	return m_yAxisLeftReverse;
}

void Graph2dScatteredWindowResultSetting::setYAxisLeftReverse(bool reverse)
{
	m_yAxisLeftReverse = reverse;
}

bool Graph2dScatteredWindowResultSetting::yAxisLeftLog() const
{
	return m_yAxisLeftLog;
}

void Graph2dScatteredWindowResultSetting::setYAxisLeftLog(bool log)
{
	m_yAxisLeftLog = log;
}

QString Graph2dScatteredWindowResultSetting::yAxisLeftTitle() const
{
	return m_yAxisLeftTitle;
}

void Graph2dScatteredWindowResultSetting::setYAxisLeftTitle(const QString title)
{
	m_yAxisLeftTitle = title;
}

bool Graph2dScatteredWindowResultSetting::yAxisRightAutoRange() const
{
	return m_yAxisRightAutoRange;
}

void Graph2dScatteredWindowResultSetting::setYAxisRightAutoRange(bool a)
{
	m_yAxisRightAutoRange = a;
}

double Graph2dScatteredWindowResultSetting::yAxisRightMin() const
{
	return m_yAxisRightMin;
}

void Graph2dScatteredWindowResultSetting::setYAxisRightMin(double min)
{
	m_yAxisRightMin = min;
}

double Graph2dScatteredWindowResultSetting::yAxisRightMax() const
{
	return m_yAxisRightMax;
}

void Graph2dScatteredWindowResultSetting::setYAxisRightMax(double max)
{
	m_yAxisRightMax = max;
}

bool Graph2dScatteredWindowResultSetting::yAxisRightReverse() const
{
	return m_yAxisRightReverse;
}

void Graph2dScatteredWindowResultSetting::setYAxisRightReverse(bool reverse)
{
	m_yAxisRightReverse = reverse;
}

bool Graph2dScatteredWindowResultSetting::yAxisRightLog() const
{
	return m_yAxisRightLog;
}

void Graph2dScatteredWindowResultSetting::setYAxisRightLog(bool log)
{
	m_yAxisRightLog = log;
}

QString Graph2dScatteredWindowResultSetting::yAxisRightTitle() const
{
	return m_yAxisRightTitle;
}

void Graph2dScatteredWindowResultSetting::setYAxisRightTitle(const QString title)
{
	m_yAxisRightTitle = title;
}

const QString& Graph2dScatteredWindowResultSetting::title() const
{
	return m_title;
}

void Graph2dScatteredWindowResultSetting::setTitle(const QString& t)
{
	m_title = t;
}

bool Graph2dScatteredWindowResultSetting::addTimeToTitle() const
{
	return m_addTimeToTitle;
}

void Graph2dScatteredWindowResultSetting::setAddTimeToTitle(bool add)
{
	m_addTimeToTitle = add;
}

QList<Graph2dWindowDataItem*> Graph2dScatteredWindowResultSetting::setupItems(Graph2dScatteredWindowResultGroupDataItem* gItem) const
{
	QList<Graph2dWindowDataItem*> ret;
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		Graph2dScatteredWindowGridResultDataItem* item = new Graph2dScatteredWindowGridResultDataItem(m_targetDatas[i], i, gItem);
		ret.append(item);
	}
	return ret;
}

std::string Graph2dScatteredWindowResultSetting::xAxis() const
{
	return m_xAxis;
}

QString Graph2dScatteredWindowResultSetting::xAxisCaption() const
{
	return m_xAxisCaption;
}

void Graph2dScatteredWindowResultSetting::setXAxis(const std::string& axis, const QString& caption)
{
	m_xAxis = axis;
	m_xAxisCaption = caption;
}

void Graph2dScatteredWindowResultSetting::setAutoXAxisLabel()
{
	QString xAxisLabel;
	if (m_xAxis == XAXIS_POSITION_X) {
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Position X");
	} else if (m_xAxis == XAXIS_POSITION_Y) {
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Position Y");
	} else if (m_xAxis == XAXIS_POSITION_Z) {
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Position Z");
	} else if (m_xAxis == XAXIS_STREAM_WISE_DISTANCE) {
		m_xAxisLabel = Graph2dScatteredWindowResultDataItem::tr("Stream-wise Distance");
	} else {
		m_xAxisLabel = m_xAxisCaption;
	}
}

QColor Graph2dScatteredWindowResultSetting::autoColor(int index)
{
	return m_colorSource->getColor(index);
}
