#include "../graph2dscatteredwindowresultsetting.h"
#include "../graph2dscatteredwindowview.h"
#include "graph2dscatteredwindowresultdataitem.h"
#include "graph2dscatteredwindowresultgroupdataitem.h"

#include <guibase/qwtplotcustomcurve.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QStandardItem>
#include <QVector3D>

#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <cmath>

Graph2dScatteredWindowResultDataItem::Graph2dScatteredWindowResultDataItem(const QString& title, int index, const Graph2dScatteredWindowResultSetting::Setting& setting, Graph2dWindowDataItem* parent) :
	Graph2dScatteredWindowDataItem(title, QIcon(":/images/iconPaper.svg"), parent)
{
	Q_UNUSED(index)

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_attached = true;
	m_curve = new QwtPlotCustomCurve(title);
	m_curve->setXAxis(QwtPlot::xBottom);

	setSetting(setting);

	Graph2dScatteredWindowView* view = dataModel()->view();
	m_curve->attach(view);
}

Graph2dScatteredWindowResultDataItem::~Graph2dScatteredWindowResultDataItem()
{
	delete m_curve;
}

QwtPlotCustomCurve* Graph2dScatteredWindowResultDataItem::curve() const
{
	return m_curve;
}

void Graph2dScatteredWindowResultDataItem::setVisible(bool visible)
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

void Graph2dScatteredWindowResultDataItem::update()
{
	updateValues();

	m_curve->setSamples(m_xValues.data(), m_yValues.data(), m_xValues.size());
}

const Graph2dScatteredWindowResultSetting::Setting& Graph2dScatteredWindowResultDataItem::setting() const
{
	return m_setting;
}

void Graph2dScatteredWindowResultDataItem::setSetting(const Graph2dScatteredWindowResultSetting::Setting& s)
{
	m_setting = s;
	m_setting.setupCurve(m_curve);
}

QString Graph2dScatteredWindowResultDataItem::title() const
{
	return m_standardItem->text();
}


const std::vector<double>& Graph2dScatteredWindowResultDataItem::xValues() const
{
	return m_xValues;
}

const std::vector<double>& Graph2dScatteredWindowResultDataItem::yValues() const
{
	return m_yValues;
}

bool Graph2dScatteredWindowResultDataItem::axisNeeded(Graph2dScatteredWindowResultSetting::AxisSide as) const
{
	return (m_setting.axisSide() == as);
}
