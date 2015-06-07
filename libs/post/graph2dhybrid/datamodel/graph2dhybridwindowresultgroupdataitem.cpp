#include "../graph2dhybridsettingdialog.h"
#include "../graph2dhybridwindow.h"
#include "../graph2dhybridwindowcontrolwidget.h"
#include "../graph2dhybridwindowdatasourcedialog.h"
#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowimportdatadataitem.h"
#include "graph2dhybridwindowimportdatagroupdataitem.h"
#include "graph2dhybridwindowresultcopygroupdataitem.h"
#include "graph2dhybridwindowresultcopygroupdataitem.h"
#include "graph2dhybridwindowresultdataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"
#include "graph2dhybridwindowrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <post/graph2d/graph2dwindowverticalaxissetting.h>

#include <QAction>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

#include <cgnslib.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_div.h>
#include <qwt_scale_engine.h>

Graph2dHybridWindowResultGroupDataItem::Graph2dHybridWindowResultGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dHybridWindowDataItem(tr("Calcuation Results"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	Graph2dHybridWindow* w = dynamic_cast<Graph2dHybridWindow*>(dataModel()->mainWindow());
	connect(w->controlWidget(), SIGNAL(indexValueChanged(int)), this, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(iValueChanged(int)), this, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(jValueChanged(int)), this, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(kValueChanged(int)), this, SLOT(sliderChanged()));
}

Graph2dHybridWindowResultGroupDataItem::~Graph2dHybridWindowResultGroupDataItem()
{

}

void Graph2dHybridWindowResultGroupDataItem::updateData(int fn)
{
	static bool updating = false;
	if (updating == true) {
		return;
	}
	updating = true;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Graph2dHybridWindowResultDataItem* item = dynamic_cast<Graph2dHybridWindowResultDataItem*>(*it);
		item->update(fn);
	}
	updating = false;
}

void Graph2dHybridWindowResultGroupDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dHybridWindowResultGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Graph2dHybridWindowResultGroupDataItem::updateChildren(const Graph2dHybridWindowResultSetting& setting)
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


void Graph2dHybridWindowResultGroupDataItem::getAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max)
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

void Graph2dHybridWindowResultGroupDataItem::showHorizontalAxisSettingDialog()
{
}

void Graph2dHybridWindowResultGroupDataItem::copy(Graph2dHybridWindowResultCopyGroupDataItem* parent)
{
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dHybridWindowResultDataItem* item = dynamic_cast<Graph2dHybridWindowResultDataItem*>(m_childItems.at(i));
		Graph2dHybridWindowResultCopyDataItem* copy = item->copy(parent);
		parent->addItem(copy);
	}
}

bool Graph2dHybridWindowResultGroupDataItem::axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const
{
	bool needed = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dHybridWindowResultDataItem* rItem = dynamic_cast<Graph2dHybridWindowResultDataItem*>(m_childItems.at(i));
		needed = needed || rItem->axisNeeded(as);
	}
	return needed;
}
