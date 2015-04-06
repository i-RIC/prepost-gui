#include "graph2dwindowmarkergroupdataitem.h"
#include "../graph2dwindowmarkersettingdialog.h"
#include "../graph2dwindowview.h"
#include "graph2dwindowmarkerdataitem.h"
#include <misc/xmlsupport.h>
#include <misc/stringtool.h>
#include <guibase/objectbrowserview.h>

#include <QMenu>
#include <QList>
#include <QAction>
#include <QStandardItem>
#include <QXmlStreamWriter>
#include <QMainWindow>
#include <QDomNode>

#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>

Graph2dWindowMarkerGroupDataItem::Graph2dWindowMarkerGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dWindowDataItem(tr("Markers"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;
	m_colorSource = 0;
}

Graph2dWindowMarkerGroupDataItem::~Graph2dWindowMarkerGroupDataItem()
{
}

void Graph2dWindowMarkerGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList list = node.childNodes();
	for (int i = 0; i < list.count(); ++i){
		QDomNode child = list.at(i);
		Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem setting;
		setting.loadFromProjectMainFile(child);
		m_setting.items().append(setting);
		Graph2dWindowMarkerDataItem* mitem = new Graph2dWindowMarkerDataItem(setting, this);
		m_childItems.append(mitem);
	}
}

void Graph2dWindowMarkerGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (int i = 0; i < m_childItems.count(); ++i){
		Graph2dWindowMarkerDataItem* mitem = dynamic_cast<Graph2dWindowMarkerDataItem*> (m_childItems.at(i));
		writer.writeStartElement("Marker");
		mitem->setting().saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Graph2dWindowMarkerGroupDataItem::propertyDialog(QWidget* parent)
{
	Graph2dWindowMarkerSettingDialog* dialog = new Graph2dWindowMarkerSettingDialog(parent);
	dialog->setSetting(m_setting);
	dialog->setXAxisLabel(m_xLabel);
	return dialog;
}

void Graph2dWindowMarkerGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog){
	Graph2dWindowMarkerSettingDialog* dialog = dynamic_cast<Graph2dWindowMarkerSettingDialog*>(propDialog);
	m_setting = dialog->setting();
	Graph2dWindowView* view = dataModel()->view();

	// delete all current children.
	dataModel()->objectBrowserView()->blockSignals(true);
	dataModel()->itemModel()->blockSignals(true);
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		delete *it;
	}
	// setup new items.
	m_childItems.append(m_setting.setupItems(this));
	updateItemMap();
	dataModel()->itemModel()->blockSignals(false);
	dataModel()->objectBrowserView()->blockSignals(false);
	dataModel()->objectBrowserView()->expandAll();
	view->replot();
}

void Graph2dWindowMarkerGroupDataItem::addMarkers(const QList<Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem>& settings)
{
	dataModel()->objectBrowserView()->blockSignals(true);
	dataModel()->itemModel()->blockSignals(true);

	m_setting.items().append(settings);
	m_childItems.append(Graph2dWindowMarkerSetting::setupItems(settings, this));
	updateItemMap();

	dataModel()->itemModel()->blockSignals(false);
	dataModel()->objectBrowserView()->blockSignals(false);
}
