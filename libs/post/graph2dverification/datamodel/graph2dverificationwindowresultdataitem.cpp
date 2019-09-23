#include "../graph2dverificationwindowresultsetting.h"
#include "../graph2dverificationwindowview.h"
#include "graph2dverificationwindowresultcopydataitem.h"
#include "graph2dverificationwindowresultcopygroupdataitem.h"
#include "graph2dverificationwindowresultdataitem.h"
#include "graph2dverificationwindowresultgroupdataitem.h"

#include <guibase/qwtplotcustomcurve.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QStandardItem>
#include <QVector3D>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

#include <cmath>

Graph2dVerificationWindowResultDataItem::Graph2dVerificationWindowResultDataItem(const QString& title, int index, const Graph2dVerificationWindowResultSetting::Setting& setting, Graph2dWindowDataItem* parent)
	: Graph2dVerificationWindowDataItem(title, QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	Q_UNUSED(index)

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_attached = true;
	m_curve = new QwtPlotCustomCurve(title);
	m_curve->setXAxis(QwtPlot::xBottom);

//	m_data = 0;
	setSetting(setting);

	Graph2dVerificationWindowView* view = dataModel()->view();
	m_curve->attach(view);
}

Graph2dVerificationWindowResultDataItem::~Graph2dVerificationWindowResultDataItem()
{
	delete m_curve;
//	if (m_data != 0){delete m_data;}
}

void Graph2dVerificationWindowResultDataItem::setVisible(bool visible)
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

void Graph2dVerificationWindowResultDataItem::update(int fn)
{
	updateValues(fn);

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

Graph2dVerificationWindowResultCopyDataItem* Graph2dVerificationWindowResultDataItem::copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent)
{
	Graph2dVerificationWindowResultCopyDataItem* ret = new Graph2dVerificationWindowResultCopyDataItem(parent);
	ret->setData(m_xValues, m_yValues);
	ret->setSetting(m_setting);
	return ret;
}

void Graph2dVerificationWindowResultDataItem::setSetting(const Graph2dVerificationWindowResultSetting::Setting& s)
{
	m_setting = s;
	m_setting.setupCurve(m_curve);
}

QString Graph2dVerificationWindowResultDataItem::title() const
{
	return m_standardItem->text();
}

bool Graph2dVerificationWindowResultDataItem::axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const
{
	return (m_setting.axisSide() == as);
}
