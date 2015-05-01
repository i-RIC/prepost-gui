#include "../graph2dwindowmarkersetting.h"
#include "../graph2dwindowview.h"
#include "graph2dwindowmarkerdataitem.h"
#include "graph2dwindowmarkergroupdataitem.h"

#include <guibase/qwtplotcustommarker.h>
#include <misc/stringtool.h>

#include <QPen>
#include <QStandardItem>
#include <QVector3D>

#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

#include <cmath>
#include <qwt_plot.h>

Graph2dWindowMarkerDataItem::Graph2dWindowMarkerDataItem(const Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item, Graph2dWindowDataItem* parent)
	: Graph2dWindowDataItem(item.label(), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;

	m_setting = item;

	m_attached = true;
	m_marker = new QwtPlotCustomMarker();
	m_marker->setLabel(item.label());
	if (item.orientation() == Graph2dWindowMarkerSetting::oHorizontal) {
		if (item.alignCenter()) {
			m_marker->setLabelAlignment(Qt::AlignRight|Qt::AlignVCenter);
		} else {
			m_marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
		}
		m_marker->setYValue(item.value());
		m_marker->setLineStyle(QwtPlotCustomMarker::HLine);
	} else {
		if (item.alignCenter()) {
			m_marker->setLabelAlignment(Qt::AlignCenter|Qt::AlignBottom);
		} else {
			m_marker->setLabelAlignment(Qt::AlignLeft|Qt::AlignBottom);
		}
		m_marker->setXValue(item.value());
		m_marker->setLabelOrientation(Qt::Vertical);
		m_marker->setLineStyle(QwtPlotCustomMarker::VLine);
	}
	m_marker->setAxes(QwtPlot::xBottom, item.axis());
	QPen pen;
	if (item.showLine()) {
		pen.setColor(item.color());
		pen.setStyle(item.penStyle());
	} else {
		pen.setColor(Qt::transparent);
	}
	m_marker->setLinePen(pen);

	pen.setColor(item.color());
	m_marker->setLabelPen(pen);

	Graph2dWindowView* view = dataModel()->view();
	m_marker->attach(view);
}

Graph2dWindowMarkerDataItem::~Graph2dWindowMarkerDataItem()
{
	delete m_marker;
}

void Graph2dWindowMarkerDataItem::setVisible(bool visible)
{
	if (visible && ! m_attached) {
		m_attached = true;
		m_marker->attach(dataModel()->view());
	}
	if (! visible && m_attached) {
		m_attached = false;
		m_marker->detach();
	}
}
