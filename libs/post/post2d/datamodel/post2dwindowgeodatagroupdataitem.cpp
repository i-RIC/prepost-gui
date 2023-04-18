#include "post2dwindowgeodatadataitem.h"
#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "../post2dwindowgraphicsview.h"
#include "private/post2dwindowgeodatagroupdataitem_scalarstocolorseditdialog.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodataproxy.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/base/iricmainwindowinterface.h>

#include <QDomNodeList>
#include <QMap>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <algorithm>

Post2dWindowGeoDataGroupDataItem::Post2dWindowGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {cond->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_condition {cond},
	m_editColorMapAction {new QAction(QIcon(":/libs/guibase/images/iconColor.svg"), Post2dWindowGeoDataGroupDataItem::tr("&Color Setting..."), this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	applyColorMapSetting();

	connect(m_editColorMapAction, &QAction::triggered, [=](bool) {editScalarsToColors();});
}

SolverDefinitionGridAttribute* Post2dWindowGeoDataGroupDataItem::condition() const
{
	return m_condition;
}

DelegatedColorMapSettingContainer* Post2dWindowGeoDataGroupDataItem::colorMapSetting() const
{
	auto typedi = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent());
	return typedi->colorMapSetting(condition()->name());
}

void Post2dWindowGeoDataGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	if (! m_condition->isReferenceInformation()) {
		menu->addSeparator();
		menu->addAction(m_editColorMapAction);
	}
}

void Post2dWindowGeoDataGroupDataItem::updateChildren()
{
	Post2dWindowGeoDataTopDataItem* tItem = dynamic_cast<Post2dWindowGeoDataTopDataItem*>(parent());
	PreProcessorGeoDataTopDataItemInterface* rtItem = tItem->preGeoDataTopDataItem();
	PreProcessorGeoDataGroupDataItemInterface* gItem = rtItem->groupDataItem(m_condition->name());

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
		PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(origChildren.at(i));
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
	auto typedi = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent());
	auto setting = typedi->colorMapSetting(condition()->name());
	if (setting != nullptr) {
		setting->applyLegendImageSetting(dataModel()->graphicsView());
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

void Post2dWindowGeoDataGroupDataItem::editScalarsToColors()
{
	auto setting = colorMapSetting();
	if (setting == nullptr) {return;}

	auto dialog = new ScalarsToColorsEditDialog(this, mainWindow());
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->setWindowTitle(tr("%1 Color Setting").arg(m_condition->caption()));
	dialog->setSetting(setting);

	dialog->show();

	iricMainWindow()->enterModelessDialogMode();
	connect(dialog, &QDialog::destroyed, [=](QObject*){iricMainWindow()->exitModelessDialogMode();});
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
