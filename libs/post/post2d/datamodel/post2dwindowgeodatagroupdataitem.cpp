#include "post2dwindowgeodatadataitem.h"
#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "../post2dwindowgraphicsview.h"
#include "private/post2dwindowgeodatagroupdataitem_scalarstocolorseditdialog.h"

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodataproxy.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/base/iricmainwindowi.h>
#include <misc/valuechangert.h>

#include <QDomNodeList>
#include <QMap>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <algorithm>

Post2dWindowGeoDataGroupDataItem::Post2dWindowGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {cond->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_condition {cond}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	applyColorMapSetting();

	auto gtItem = gridTypeDataItem();
	m_toolBarWidgetController = gtItem->createToolBarWidgetController(cond->name(), mainWindow());
}

Post2dWindowGeoDataGroupDataItem::~Post2dWindowGeoDataGroupDataItem()
{
	delete m_toolBarWidgetController;
}

SolverDefinitionGridAttribute* Post2dWindowGeoDataGroupDataItem::condition() const
{
	return m_condition;
}

bool Post2dWindowGeoDataGroupDataItem::colorBarShouldBeVisible() const
{
	if (! isAncientChecked()) {return false;}
	if (m_standardItem->checkState() == Qt::Unchecked) {return false;}

	for (auto child : m_childItems) {
		if (child->standardItem()->checkState() == Qt::Checked) {return true;}
	}

	return false;
}

DelegatedColorMapSettingContainer* Post2dWindowGeoDataGroupDataItem::colorMapSetting() const
{
	auto typedi = gridTypeDataItem();
	return typedi->colorMapSetting(condition()->name());
}

void Post2dWindowGeoDataGroupDataItem::addCustomMenuItems(QMenu* /*menu*/)
{}

void Post2dWindowGeoDataGroupDataItem::updateChildren()
{
	Post2dWindowGeoDataTopDataItem* tItem = dynamic_cast<Post2dWindowGeoDataTopDataItem*>(parent());
	PreProcessorGeoDataTopDataItemI* rtItem = tItem->preGeoDataTopDataItem();
	PreProcessorGeoDataGroupDataItemI* gItem = rtItem->groupDataItem(m_condition->name());

	std::vector<GraphicsWindowDataItem*> oldChildren = m_childItems;
	QMap<GeoData*, Post2dWindowGeoDataDataItem*> map;
	for (int i = 0; i < oldChildren.size(); ++i) {
		Post2dWindowGeoDataDataItem* item = dynamic_cast<Post2dWindowGeoDataDataItem*>(oldChildren.at(i));
		map.insert(item->geoDataProxy()->geoData(), item);
		m_standardItem->takeRow(0);
	}
	m_childItems.clear();
	m_itemNameMap.clear();

	std::vector<GraphicsWindowDataItem*> origChildren = gItem->childItems();
	for (int i = 0; i < origChildren.size(); ++i) {
		PreProcessorGeoDataDataItemI* item = dynamic_cast<PreProcessorGeoDataDataItemI*>(origChildren.at(i));
		GeoData* geoData = item->geoData();
		if (geoData == nullptr) {continue;}

		if (map.contains(geoData)) {
			m_childItems.push_back(map.value(geoData));
			m_standardItem->appendRow(map.value(geoData)->standardItem());
			auto it = std::find(oldChildren.begin(), oldChildren.end(), map.value(geoData));
			oldChildren.erase(it);
			m_itemNameMap.insert(geoData->name(), map.value(geoData));
		} else {
			// try to add.
			GeoDataProxy* proxy = geoData->getProxy();
			if (proxy != nullptr) {
				connect(geoData, &GeoData::updateActorSettingExecuted, proxy, &GeoDataProxy::updateActorSetting);
				Post2dWindowGeoDataDataItem* pItem = new Post2dWindowGeoDataDataItem(this);
				pItem->setGeoDataProxy(proxy);
				proxy->setupDataItem();
				m_childItems.push_back(pItem);
				m_itemNameMap.insert(geoData->name(), pItem);
				connect(item, SIGNAL(captionChanged(QString)), pItem, SLOT(updateCaption(QString)));
			}
		}
	}
	for (int i = 0; i < oldChildren.size(); ++i) {
		delete oldChildren.at(i);
	}
	assignActorZValues(m_zDepthRange);
	updateItemMap();
}

void Post2dWindowGeoDataGroupDataItem::applyColorMapSetting()
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowGeoDataDataItem*>(child);
		item->applyColorMapSetting();
	}
	auto typedi = gridTypeDataItem();
	auto setting = typedi->colorMapSetting(condition()->name());
	if (setting != nullptr) {
		setting->activeImageSetting()->apply(dataModel()->graphicsView());
	}
}

void Post2dWindowGeoDataGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSetting();
	if (setting == nullptr) {return;}
	if (setting->usePreSetting) {return;}

	setting->customSetting->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
}

void Post2dWindowGeoDataGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSetting();
	if (setting == nullptr) {return;}
	if (setting->usePreSetting) {return;}

	setting->customSetting->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void Post2dWindowGeoDataGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSetting();
	if (setting == nullptr) {return;}
	if (setting->usePreSetting) {return;}

	setting->customSetting->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
}

void Post2dWindowGeoDataGroupDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	gridTypeDataItem()->updateColorBarVisibility(condition()->name());
}

bool Post2dWindowGeoDataGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	bool added = false;

	if (m_toolBarWidgetController != nullptr) {
		auto widget = m_toolBarWidgetController->widget();
		widget->setParent(toolBar);
		widget->show();
		toolBar->addWidget(widget);
		added = true;
	}

	return added;
}

void Post2dWindowGeoDataGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowGeoDataGroupDataItem::propertyDialog(QWidget* parent)
{
	auto setting = colorMapSetting();
	if (setting == nullptr) {return nullptr;}

	auto dialog = new ScalarsToColorsEditDialog(this, parent);
	dialog->setWindowTitle(tr("%1 Color Setting").arg(m_condition->caption()));
	dialog->setSetting(setting);

	return dialog;
}

Post2dWindowGridTypeDataItem* Post2dWindowGeoDataGroupDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent());
}

void Post2dWindowGeoDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement elem = children.at(i).toElement();
		QString name = elem.attribute("name");
		if (m_itemNameMap.contains(name)) {
			Post2dWindowGeoDataDataItem* item = m_itemNameMap.value(name);
			item->loadFromProjectMainFile(elem);
		}
	}
	updateChildren();
}

void Post2dWindowGeoDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_condition->name().c_str());
	for (auto child : m_childItems) {
		writer.writeStartElement("GeoData");
		child->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowGeoDataGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);
	gridTypeDataItem()->updateColorBarVisibility(condition()->name());
}
