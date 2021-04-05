#include "post3dwindowcellcontourdataitem.h"
#include "post3dwindowcellcontourgroupsettingdialog.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowcellcontourgroupdataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowcellcontourgroupdataitem_setsettingcommand.h"

#include <guibase/graphicsmisc.h>
#include <guibase/objectbrowserview.h>
#include <guibase/vtkCustomScalarBarActor.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>

#include <QDomNodeList>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkScalarBarWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

Post3dWindowCellContourGroupDataItem::Post3dWindowCellContourGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	setupScalarBarActor();
}

Post3dWindowCellContourGroupDataItem::~Post3dWindowCellContourGroupDataItem()
{
	m_scalarBarWidget->SetInteractor(nullptr);
	m_scalarBarWidget->Delete();
}

void Post3dWindowCellContourGroupDataItem::setSetting(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer>& rangeSettings)
{
	clearChildItems();

	auto tItem = dynamic_cast<Post3dWindowCellContourGroupTopDataItem*> (parent());
	m_setting = scalarSetting;

	int idx = 1;
	for (auto rs : rangeSettings) {
		auto label = tr("Range%1").arg(idx);
		auto item = new Post3dWindowCellContourDataItem(label, this);
		item->setSetting(rs);
		item->updateZScale(tItem->zScale());

		m_childItems.push_back(item);
		++ idx;
	}

	updateItemMap();
	updateActorSettings();

	ObjectBrowserView* oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

const ScalarSettingContainer& Post3dWindowCellContourGroupDataItem::setting() const
{
	return m_setting;
}

void Post3dWindowCellContourGroupDataItem::update()
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post3dWindowCellContourDataItem*> (c);
		item->update();
	}
}

LookupTableContainer* Post3dWindowCellContourGroupDataItem::lookupTable() const
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	return gtItem->cellLookupTable(iRIC::toStr(m_setting.target));
}

void Post3dWindowCellContourGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "RangeSetting") {
			QString label = childElem.attribute("label");
			auto item = new Post3dWindowCellContourDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}

	update();
	updateScalarBarActorSetting();
}

void Post3dWindowCellContourGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	m_setting.save(writer);

	for (int i = 0; i < m_childItems.size(); ++i) {
		auto fitem = dynamic_cast<Post3dWindowCellContourDataItem*>(m_childItems.at(i));
		writer.writeStartElement("RangeSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post3dWindowCellContourGroupDataItem::propertyDialog(QWidget* p)
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	auto tItem = dynamic_cast<Post3dWindowCellContourGroupTopDataItem*> (parent());
	auto zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (zoneData == nullptr) {return nullptr;}
	if (zoneData->data() == nullptr) {return nullptr;}

	std::vector<Post3dCellRangeSettingContainer> rangeSettings;
	for (auto c : m_childItems) {
		auto c2 = dynamic_cast<Post3dWindowCellContourDataItem*> (c);
		rangeSettings.push_back(c2->setting());
	}

	auto dialog = new Post3dWindowCellContourGroupSettingDialog(p);
	dialog->setGridTypeDataItem(gtItem);
	dialog->setZoneData(zoneData);
	auto *lookupTable = gtItem->cellLookupTable(iRIC::toStr(m_setting.target));
	dialog->setLookupTable(*lookupTable);
	dialog->setScalarSetting(m_setting);
	dialog->setRangeSettings(rangeSettings);
	dialog->setColorBarTitleMap(tItem->m_colorBarTitleMap);

	return dialog;
}

void Post3dWindowCellContourGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<Post3dWindowCellContourGroupSettingDialog*> (propDialog);
	auto scalarSetting = dialog->scalarSetting();
	auto rangeSettings = dialog->rangeSettings();
	auto lookupTable = dialog->lookupTable();
	auto scalarBarTitle = dialog->scalarBarTitle();

	pushRenderCommand(new SetSettingCommand(scalarSetting, rangeSettings, lookupTable, scalarBarTitle, this), this);
}

void Post3dWindowCellContourGroupDataItem::updateActorSettings()
{
	updateScalarBarActorSetting();
}

void Post3dWindowCellContourGroupDataItem::setupScalarBarActor()
{
	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkScalarBarWidget::New();
	m_scalarBarWidget->SetScalarBarActor(vtkCustomScalarBarActor::New());
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetEnabled(0);
	m_scalarBarWidget->SetInteractor(iren);

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateScalarBarActorSetting();
}

void Post3dWindowCellContourGroupDataItem::updateScalarBarActorSetting()
{
	auto lookup = lookupTable();
	if (lookup == nullptr) {return;}

	auto a = m_scalarBarWidget->GetScalarBarActor();
	std::string targetStr = iRIC::toStr(m_setting.target);

	auto topitem = dynamic_cast<Post3dWindowCellContourGroupTopDataItem*>(parent());
	a->SetTitle(iRIC::toStr(topitem->m_colorBarTitleMap.value(targetStr)).c_str());
	a->SetLookupTable(lookup->vtkObj());

	m_standardItem->setText(topitem->m_colorBarTitleMap.value(targetStr));

	auto& s = m_setting.scalarBarSetting;

	a->SetNumberOfLabels(s.numberOfLabels);
	s.titleTextSetting.applySetting(a->GetTitleTextProperty());
	s.labelTextSetting.applySetting(a->GetLabelTextProperty());
	a->SetMaximumNumberOfColors(256);
}
