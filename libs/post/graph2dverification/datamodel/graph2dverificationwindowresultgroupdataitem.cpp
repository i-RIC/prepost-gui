#include "../graph2dverificationsettingdialog.h"
#include "../graph2dverificationwindow.h"
#include "../graph2dverificationwindowcontrolwidget.h"
#include "../graph2dverificationwindowdatasourcedialog.h"
#include "../graph2dverificationwindowview.h"
#include "graph2dverificationwindowimportdatadataitem.h"
#include "graph2dverificationwindowimportdatagroupdataitem.h"
#include "graph2dverificationwindowresultcopygroupdataitem.h"
#include "graph2dverificationwindowresultcopygroupdataitem.h"
#include "graph2dverificationwindowresultdataitem.h"
#include "graph2dverificationwindowresultgroupdataitem.h"
#include "graph2dverificationwindowrootdataitem.h"

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

Graph2dVerificationWindowResultGroupDataItem::Graph2dVerificationWindowResultGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dVerificationWindowDataItem(tr("Calculation Results"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	Graph2dVerificationWindow* w = dynamic_cast<Graph2dVerificationWindow*>(dataModel()->mainWindow());
	connect(w->controlWidget(), SIGNAL(indexValueChanged(int)), this, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(iValueChanged(int)), this, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(jValueChanged(int)), this, SLOT(sliderChanged()));
	connect(w->controlWidget(), SIGNAL(kValueChanged(int)), this, SLOT(sliderChanged()));
}

Graph2dVerificationWindowResultGroupDataItem::~Graph2dVerificationWindowResultGroupDataItem()
{

}

void Graph2dVerificationWindowResultGroupDataItem::updateData(int fn)
{
	static bool updating = false;
	if (updating == true) {
		return;
	}
	updating = true;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Graph2dVerificationWindowResultDataItem* item = dynamic_cast<Graph2dVerificationWindowResultDataItem*>(*it);
		item->update(fn);
	}
	updating = false;
}

void Graph2dVerificationWindowResultGroupDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dVerificationWindowResultGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Graph2dVerificationWindowResultGroupDataItem::updateChildren(const Graph2dVerificationWindowResultSetting& setting)
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


void Graph2dVerificationWindowResultGroupDataItem::getAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max)
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

void Graph2dVerificationWindowResultGroupDataItem::showHorizontalAxisSettingDialog()
{
}

void Graph2dVerificationWindowResultGroupDataItem::copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent)
{
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dVerificationWindowResultDataItem* item = dynamic_cast<Graph2dVerificationWindowResultDataItem*>(m_childItems.at(i));
		Graph2dVerificationWindowResultCopyDataItem* copy = item->copy(parent);
		parent->addItem(copy);
	}
}

bool Graph2dVerificationWindowResultGroupDataItem::axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const
{
	bool needed = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dVerificationWindowResultDataItem* rItem = dynamic_cast<Graph2dVerificationWindowResultDataItem*>(m_childItems.at(i));
		needed = needed || rItem->axisNeeded(as);
	}
	return needed;
}
