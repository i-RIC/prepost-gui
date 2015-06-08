#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowimportdatadataitem.h"
#include "graph2dhybridwindowimportdatagroupdataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"
#include "graph2dhybridwindowrootdataitem.h"

#include <guibase/qwtplotcustomcurve.h>
#include <misc/xmlsupport.h>

#include <QStandardItem>

#include <qwt_plot.h>

Graph2dHybridWindowImportDataDataItem::Graph2dHybridWindowImportDataDataItem(Graph2dWindowDataItem* parent)
	: Graph2dHybridWindowDataItem("" , QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_attached = true;
	m_curve = new QwtPlotCustomCurve("");
	m_curve->setXAxis(QwtPlot::xBottom);
	m_curve->setYAxis(QwtPlot::yLeft);
	Graph2dHybridWindowView* view = dataModel()->view();
	m_curve->attach(view);
}

Graph2dHybridWindowImportDataDataItem::~Graph2dHybridWindowImportDataDataItem()
{
	delete m_curve;
}

void Graph2dHybridWindowImportDataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_attached = iRIC::getBooleanAttribute(node, "attached");
	m_index = iRIC::getIntAttribute(node, "index");
	m_filename = elem.attribute("filename");
	m_name = elem.attribute("name");

	QDomNode sNode = iRIC::getChildNode(node, "Setting");
	if (! sNode.isNull()) {
		m_setting.loadFromProjectMainFile(sNode);
	}
	setSetting(m_setting);

	QDomNode xNode = iRIC::getChildNode(node, "XValues");
	if (! xNode.isNull()) {
		m_xValues.clear();
		for (int i = 0; i < xNode.childNodes().count(); ++i) {
			QDomNode n = xNode.childNodes().at(i);
			QString arg = n.toElement().text();
			m_xValues.append(arg.toDouble());
		}
	}
	QDomNode yNode = iRIC::getChildNode(node, "YValues");
	if (! yNode.isNull()) {
		m_yValues.clear();
		for (int i = 0; i < yNode.childNodes().count(); ++i) {
			QDomNode n = yNode.childNodes().at(i);
			QString arg = n.toElement().text();
			m_yValues.append(arg.toDouble());
		}
	}

}

void Graph2dHybridWindowImportDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setBooleanAttribute(writer, "attached", m_attached);
	iRIC::setIntAttribute(writer, "index", m_index);
	writer.writeAttribute("filename", m_filename);
	writer.writeAttribute("name", m_name);

	writer.writeStartElement("Setting");
	m_setting.saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("XValues");
	for (int i = 0; i < m_xValues.count(); ++i) {
		double val = m_xValues.at(i);
		writer.writeTextElement("Value", QString::number(val));
	}
	writer.writeEndElement();

	writer.writeStartElement("YValues");
	for (int i = 0; i < m_yValues.count(); ++i) {
		double val = m_yValues.at(i);
		writer.writeTextElement("Value", QString::number(val));
	}
	writer.writeEndElement();
}

void Graph2dHybridWindowImportDataDataItem::setIndex(int index)
{
	m_index = index;
}

void Graph2dHybridWindowImportDataDataItem::setData(const QVector<double>& xvals, const QVector<double>& yvals)
{
	m_xValues = xvals;
	m_yValues = yvals;
}

void Graph2dHybridWindowImportDataDataItem::setTitle(const QString& title)
{
	m_standardItem->setText(title);
	m_standardItemCopy = m_standardItem->clone();
	m_curve->setTitle(title);
	m_setting.setName(title);
}

void Graph2dHybridWindowImportDataDataItem::setVisible(bool visible)
{
	if (visible && ! m_attached) {
		m_attached = true;
		m_curve->attach(dataModel()->view());
	}
	if (! visible && m_attached) {
		m_attached = false;
		m_curve->detach();
	}
}

void Graph2dHybridWindowImportDataDataItem::setSetting(const Graph2dHybridWindowResultSetting::Setting& s)
{
	m_setting = s;
	m_setting.setupCurve(m_curve);
	setTitle(s.name());
}

void Graph2dHybridWindowImportDataDataItem::update()
{
	QVector<double> xvals;
	QVector<double> yvals;

	const Graph2dHybridWindowResultSetting& s = dataModel()->setting();
	Graph2dHybridWindowDataItem::buildData(
		m_xValues, m_yValues, s, m_setting, xvals, yvals);

	double* x, *y;
	buildXY(xvals, yvals, &x, &y);

	m_curve->setSamples(x, y, xvals.count());
	delete x;
	delete y;
}

void Graph2dHybridWindowImportDataDataItem::setId(const QString filename, const QString name)
{
	m_filename = filename;
	m_name = name;
	setTitle(id());
}

const QString Graph2dHybridWindowImportDataDataItem::id()
{
	QString temp = tr("%1 - %2");

	return temp.arg(m_filename).arg(m_name);
}

const QString Graph2dHybridWindowImportDataDataItem::title() const
{
	return m_standardItem->text();
}

bool Graph2dHybridWindowImportDataDataItem::axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const
{
	return (m_setting.axisSide() == as);
}
