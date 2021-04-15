#include "post3dwindowcellcontourgroupdataitem.h"
#include "post3dwindowcellcontourgroupsettingdialog.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "post3dwindowcellrangesettingwidget.h"
#include "../post3dcellrangesettingcontainer.h"
#include "private/post3dwindowcellcontourgrouptopdataitem_createcommand.h"

#include <guibase/objectbrowserview.h>
#include <guibase/scalarsettingcontainer.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QMenu>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include <vtkCellData.h>

Post3dWindowCellContourGroupTopDataItem::Post3dWindowCellContourGroupTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Contours (cell center)"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetCellData())) {
		m_colorBarTitleMap.insert(name, name.c_str());
	}
}

double Post3dWindowCellContourGroupTopDataItem::zScale() const
{
	return m_zScale;
}

void Post3dWindowCellContourGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowCellContourGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowCellContourGroupDataItem*> (item);
		child->update();
	}
}

void Post3dWindowCellContourGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarBarTitles") {
			QDomNodeList titles = children.at(i).childNodes();
			for (int i = 0; i < titles.count(); ++i) {
				std::string val = iRIC::toStr(titles.at(i).toElement().attribute("value"));
				QString title = titles.at(i).toElement().attribute("title");
				m_colorBarTitleMap[val] = title;
			}
		} else if (childElem.nodeName() == "ContourGroup") {
			auto item = new Post3dWindowCellContourGroupDataItem(this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(children.at(i));
			m_childItems.push_back(item);
		}
	}
}

void Post3dWindowCellContourGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// scalar bar titles
	writer.writeStartElement("ScalarBarTitles");
	QMapIterator<std::string, QString> i(m_colorBarTitleMap);
	while (i.hasNext()) {
		i.next();
		if (i.key().size() > 0) {
			writer.writeStartElement("ScalarBarTitle");
			writer.writeAttribute("value", i.key().c_str());
			writer.writeAttribute("title", i.value());
			writer.writeEndElement();
		}
	}
	writer.writeEndElement();

	// contours
	for (auto item : m_childItems) {
		writer.writeStartElement("ContourGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowCellContourGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}


QDialog* Post3dWindowCellContourGroupTopDataItem::addDialog(QWidget* p)
{
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return nullptr;
	}

	if (childItems().size() >= 4) {
		QMessageBox::warning(postProcessorWindow(), tr("Warning"), tr("A maximum of four contours may be defined."));
		return nullptr;
	}

	auto dialog = new Post3dWindowCellContourGroupSettingDialog(p);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	dialog->setZoneData(zoneData);

	ScalarSettingContainer scalarSetting;
	scalarSetting.target = zoneData->data()->GetCellData()->GetArrayName(0);
	switch (childItems().size() % 4) {
	case 0:
		scalarSetting.scalarBarSetting.positionX = 0.8;
		scalarSetting.scalarBarSetting.positionY = 0.1;
		break;
	case 1:
		scalarSetting.scalarBarSetting.positionX = 0.1;
		scalarSetting.scalarBarSetting.positionY = 0.1;
		break;
	case 2:
		scalarSetting.scalarBarSetting.positionX = 0.1;
		scalarSetting.scalarBarSetting.positionY = 0.6;
		break;
	case 3:
		scalarSetting.scalarBarSetting.positionX = 0.8;
		scalarSetting.scalarBarSetting.positionY = 0.6;
		break;
	}
	dialog->setScalarSetting(scalarSetting);

	std::vector<Post3dCellRangeSettingContainer> rangeSettings;
	dialog->setRangeSettings(rangeSettings);

	auto *lookupTable = gtItem->cellLookupTable(iRIC::toStr(scalarSetting.target));
	dialog->setLookupTable(*lookupTable);
	dialog->setColorBarTitleMap(m_colorBarTitleMap);

	return dialog;
}

void Post3dWindowCellContourGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<Post3dWindowCellContourGroupSettingDialog*> (propDialog);
	auto scalarSetting = dialog->scalarSetting();
	auto rangeSettings = dialog->rangeSettings();
	auto lookupTable = dialog->lookupTable();
	auto scalarBarTitle = dialog->scalarBarTitle();

	pushRenderCommand(new CreateCommand(scalarSetting, rangeSettings, lookupTable, scalarBarTitle, this), this);
}
