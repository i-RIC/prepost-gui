#include "../graph2dscatteredwindowview.h"
#include "graph2dscatteredwindowresultdataitem.h"
#include "graph2dscatteredwindowresultgroupdataitem.h"

#include <QIcon>
#include <QStandardItem>

#include <qwt_scale_div.h>

Graph2dScatteredWindowResultGroupDataItem::Graph2dScatteredWindowResultGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dScatteredWindowDataItem(tr("Calcuation Results"), QIcon(":/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;
}

Graph2dScatteredWindowResultGroupDataItem::~Graph2dScatteredWindowResultGroupDataItem()
{

}

void Graph2dScatteredWindowResultGroupDataItem::updateData(int fn)
{
	static bool updating = false;
	if (updating == true) {
		return;
	}
	updating = true;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Graph2dScatteredWindowResultDataItem* item = dynamic_cast<Graph2dScatteredWindowResultDataItem*>(*it);
		item->update(fn);
	}
	updating = false;
}

void Graph2dScatteredWindowResultGroupDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dScatteredWindowResultGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Graph2dScatteredWindowResultGroupDataItem::updateChildren(const Graph2dScatteredWindowResultSetting& setting)
{
	// delete all current children.
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		delete *it;
	}
	m_childItems.clear();
	updateItemMap();
	// setup new items.
	m_childItems.append(setting.setupItems(this));
}

void Graph2dScatteredWindowResultGroupDataItem::getAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max)
{
	Graph2dWindowView* view = dataModel()->view();
	QwtScaleDiv sDiv;
	if (as == Graph2dWindowDataModel::asLeft) {
		sDiv = view->axisScaleDiv(QwtPlot::yLeft);
	} else {
		sDiv = view->axisScaleDiv(QwtPlot::yRight);
	}
	*min = sDiv.lowerBound();
	*max = sDiv.upperBound();
}

bool Graph2dScatteredWindowResultGroupDataItem::axisNeeded(Graph2dScatteredWindowResultSetting::AxisSide as) const
{
	bool needed = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dScatteredWindowResultDataItem* rItem = dynamic_cast<Graph2dScatteredWindowResultDataItem*>(m_childItems.at(i));
		needed = needed || rItem->axisNeeded(as);
	}
	return needed;
}
