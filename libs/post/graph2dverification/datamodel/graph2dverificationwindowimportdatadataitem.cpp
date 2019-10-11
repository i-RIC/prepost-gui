#if SKIP
#include "../graph2dverificationwindowview.h"
#include "graph2dverificationwindowimportdatadataitem.h"
#include "graph2dverificationwindowimportdatagroupdataitem.h"
#include "graph2dverificationwindowresultgroupdataitem.h"
#include "graph2dverificationwindowrootdataitem.h"

#include <guibase/qwtplotcustomcurve.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QFileInfo>
#include <QStandardItem>

#include <qwt_plot.h>

Graph2dVerificationWindowImportDataDataItem::Graph2dVerificationWindowImportDataDataItem(Graph2dWindowDataItem* parent)
	: Graph2dVerificationWindowDataItem("" , QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_attached = true;
	m_curve = new QwtPlotCustomCurve("");
	m_curve->setXAxis(QwtPlot::xBottom);
	m_curve->setYAxis(QwtPlot::yLeft);
	Graph2dVerificationWindowView* view = dataModel()->view();
	m_curve->attach(view);
}

Graph2dVerificationWindowImportDataDataItem::~Graph2dVerificationWindowImportDataDataItem()
{
	delete m_curve;
}

void Graph2dVerificationWindowImportDataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void Graph2dVerificationWindowImportDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

void Graph2dVerificationWindowImportDataDataItem::setIndex(int index)
{
	m_index = index;
}

void Graph2dVerificationWindowImportDataDataItem::setData(const QVector<double>& xvals, const QVector<double>& yvals)
{
	m_xValues = xvals;
	m_yValues = yvals;
}

void Graph2dVerificationWindowImportDataDataItem::setTitle(const QString& title)
{
	m_standardItem->setText(title);
	m_standardItemCopy = m_standardItem->clone();
	m_curve->setTitle(title);
	m_setting.setName(title);
}

void Graph2dVerificationWindowImportDataDataItem::setVisible(bool visible)
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

void Graph2dVerificationWindowImportDataDataItem::setSetting(const Graph2dVerificationWindowResultSetting::Setting& s)
{
	m_setting = s;
	m_setting.setupCurve(m_curve);
	setTitle(s.name());
}

void Graph2dVerificationWindowImportDataDataItem::update()
{
	QVector<double> xvals;
	QVector<double> yvals;

	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
	Graph2dVerificationWindowDataItem::buildData(
		m_xValues, m_yValues, s, m_setting, xvals, yvals);

	double* x, *y;
	buildXY(xvals, yvals, &x, &y);

	m_curve->setSamples(x, y, xvals.count());
	delete x;
	delete y;
}

void Graph2dVerificationWindowImportDataDataItem::setId(const QString filename, const QString name)
{
	m_filename = filename;
	m_name = name;
	setTitle(id());
}

QString Graph2dVerificationWindowImportDataDataItem::id() const
{
	QString temp = tr("%1 - %2");

	return temp.arg(m_filename).arg(m_name);
}

QString Graph2dVerificationWindowImportDataDataItem::title() const
{
	return m_standardItem->text();
}

bool Graph2dVerificationWindowImportDataDataItem::axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const
{
	return (m_setting.axisSide() == as);
}
#endif
