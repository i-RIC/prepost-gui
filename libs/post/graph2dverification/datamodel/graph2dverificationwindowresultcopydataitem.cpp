#if SKIP
#include "../graph2dverificationwindowview.h"
#include "graph2dverificationwindowresultcopydataitem.h"
#include "graph2dverificationwindowresultdataitem.h"
#include "graph2dverificationwindowresultgroupdataitem.h"
#include "graph2dverificationwindowrootdataitem.h"

#include <guibase/qwtplotcustomcurve.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QStandardItem>

#include <qwt_plot.h>

Graph2dVerificationWindowResultCopyDataItem::Graph2dVerificationWindowResultCopyDataItem(Graph2dWindowDataItem* parent)
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
	m_data = 0;
	Graph2dVerificationWindowView* view = dataModel()->view();
	m_curve->attach(view);
}

Graph2dVerificationWindowResultCopyDataItem::~Graph2dVerificationWindowResultCopyDataItem()
{
	delete m_curve;
	delete m_data;
}

void Graph2dVerificationWindowResultCopyDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_attached = iRIC::getBooleanAttribute(node, "attached");
	m_index = iRIC::getIntAttribute(node, "index");
	m_idType = static_cast<IDType>(iRIC::getIntAttribute(node, "idType"));

	QDomNode sNode = iRIC::getChildNode(node, "Setting");
	if (! sNode.isNull()) {
		m_setting.loadFromProjectMainFile(sNode);
	}
	setSetting(m_setting);

	QDomNode aNode = iRIC::getChildNode(node, "IdArgs");
	if (! aNode.isNull()) {
		m_idArgs.clear();
		for (int i = 0; i < aNode.childNodes().count(); ++i) {
			QDomNode n = aNode.childNodes().at(i);
			QString arg = n.toElement().text();
			m_idArgs.append(arg);
		}
	}
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

void Graph2dVerificationWindowResultCopyDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setBooleanAttribute(writer, "attached", m_attached);
	iRIC::setIntAttribute(writer, "index", m_index);
	iRIC::setIntAttribute(writer, "idType", m_idType);

	writer.writeStartElement("Setting");
	m_setting.saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("IdArgs");
	for (int i = 0; i < m_idArgs.count(); ++i) {
		QString arg = m_idArgs.at(i);
		writer.writeTextElement("Arg", arg);
	}
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

void Graph2dVerificationWindowResultCopyDataItem::setIndex(int index)
{
	m_index = index;
}

void Graph2dVerificationWindowResultCopyDataItem::setData(const QVector<double>& xvals, const QVector<double>& yvals)
{
	m_xValues = xvals;
	m_yValues = yvals;
}

void Graph2dVerificationWindowResultCopyDataItem::setTitle(const QString& title)
{
	m_standardItem->setText(title);
	m_standardItemCopy = m_standardItem->clone();
	m_curve->setTitle(title);
	m_setting.setName(title);
}

void Graph2dVerificationWindowResultCopyDataItem::setVisible(bool visible)
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

void Graph2dVerificationWindowResultCopyDataItem::setSetting(const Graph2dVerificationWindowResultSetting::Setting& s)
{
	m_setting = s;
	m_setting.setupCurve(m_curve);
	setTitle(s.name());
}

void Graph2dVerificationWindowResultCopyDataItem::update()
{
	QVector<double> xvals;
	QVector<double> yvals;

	if (m_data != 0) {
		delete m_data;
		m_data = 0;
	}
	const Graph2dVerificationWindowResultSetting& s = dataModel()->setting();
	Graph2dVerificationWindowDataItem::buildData(
		m_xValues, m_yValues, s, m_setting, xvals, yvals);

	double* x, *y;
	buildXY(xvals, yvals, &x, &y);

//	m_data = new QwtArrayData(x, y, xvals.count());
	m_curve->setSamples(x, y, xvals.count());
	delete x;
	delete y;
//	m_curve->setData(m_data);
}

void Graph2dVerificationWindowResultCopyDataItem::setId(IDType idtype, const QStringList args)
{
	m_idType = idtype;
	m_idArgs = args;
	setTitle(id());
}

QString Graph2dVerificationWindowResultCopyDataItem::id() const
{
	QString temp;

	switch (m_idType) {
	case idtTimeBaseIterative:
		temp = tr("%1");
		break;
	case idtTime1D:
		temp = tr("%1 (I = %2)");
		break;
	case idtTime2D:
		temp = tr("%1 (I = %2, J = %3)");
		break;
	case idtTime3D:
		temp = tr("%1 (I = %2, J = %3, K = %4)");
		break;
	case idtTimeUnst:
		temp = tr("%1 (Index = %2)");
		break;
	case idt1D:
		temp = tr("%1 (t = %2)");
		break;
	case idtI2D:
		temp = tr("%1 (t = %2, J = %3)");
		break;
	case idtI3D:
		temp = tr("%1 (t = %2, J = %3, K = %4)");
		break;
	case idtJ2D:
		temp = tr("%1 (t = %2, I = %3)");
		break;
	case idtJ3D:
		temp = tr("%1 (t = %2, I = %3, K = %4)");
		break;
	case idtK3D:
		temp = tr("%1 (t = %2, I = %3, J = %4)");
		break;
	}
	for (int i = 0; i < m_idArgs.count(); ++i) {
		QString a = m_idArgs.at(i);
		temp = temp.arg(a);
	}
	return temp;
}

QString Graph2dVerificationWindowResultCopyDataItem::title() const
{
	return m_standardItem->text();
}

bool Graph2dVerificationWindowResultCopyDataItem::axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const
{
	return (m_setting.axisSide() == as);
}
#endif
