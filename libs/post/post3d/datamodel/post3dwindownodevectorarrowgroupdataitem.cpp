#include "post3dwindownodevectorarrowdataitem.h"
#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindownodevectorarrowsettingdialog.h"
#include "post3dwindownodevectorarrowtopdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodevectorarrowgroupdataitem_setsettingcommand.h"

#include <guibase/objectbrowserview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QStandardItem>
#include <QXmlStreamWriter>

Post3dWindowNodeVectorArrowGroupDataItem::Post3dWindowNodeVectorArrowGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"test", QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor2D(m_legendActors.arrowActor());
	renderer()->AddActor2D(m_legendActors.textActor());

	m_legendActors.setPosition(0.75, 0.02);
	m_legendActors.setColor(0, 0, 0);
}

Post3dWindowNodeVectorArrowGroupDataItem::~Post3dWindowNodeVectorArrowGroupDataItem()
{
	renderer()->RemoveActor2D(m_legendActors.arrowActor());
	renderer()->RemoveActor2D(m_legendActors.textActor());
}

void Post3dWindowNodeVectorArrowGroupDataItem::setSetting(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings)
{
	clearChildItems();

	Post3dWindowNodeVectorArrowTopDataItem* tItem = dynamic_cast<Post3dWindowNodeVectorArrowTopDataItem*> (parent());
	m_setting = arrowSetting;
	if (faceSettings.size() > 0) {
		// use the arrow size and width setting of the first item;
		const auto& first = faceSettings.at(0).arrowSetting;
		m_setting.arrowSize = first.arrowSize;
		m_setting.lineWidth = first.lineWidth;
	}

	for (auto fs : faceSettings) {
		auto item = new Post3dWindowNodeVectorArrowDataItem(this);
		item->setSetting(fs);
		item->updateZScale(tItem->zScale());

		m_childItems.push_back(item);
	}

	updateItemMap();
	updateActorSettings();

	ObjectBrowserView* oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

const ArrowSettingContainer& Post3dWindowNodeVectorArrowGroupDataItem::setting() const
{
	return m_setting;
}

void Post3dWindowNodeVectorArrowGroupDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*> (item);
		child->update();
	}
}

void Post3dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "Arrow") {
			auto item = new Post3dWindowNodeVectorArrowDataItem(this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}

	updateActorSettings();
}

void Post3dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);

	for (auto item : m_childItems) {
		writer.writeStartElement("Arrow");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post3dWindowNodeVectorArrowGroupDataItem::propertyDialog(QWidget* p)
{
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (zoneData == nullptr) {return nullptr;}
	if (zoneData->data() == nullptr) {return nullptr;}

	std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting> faceSettings;
	for (auto child : m_childItems) {
		auto child2 = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*> (child);
		faceSettings.push_back(child2->setting());
	}

	auto dialog = new Post3dWindowNodeVectorArrowSettingDialog(p);
	dialog->setZoneData(zoneData);
	dialog->setArrowSetting(m_setting);
	dialog->setFaceSettings(faceSettings);
	return dialog;
}

void Post3dWindowNodeVectorArrowGroupDataItem::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<Post3dWindowNodeVectorArrowSettingDialog*> (d);

	pushRenderCommand(new SetSettingCommand(dialog->arrowSetting(), dialog->faceSettings(), this), this);
}

void Post3dWindowNodeVectorArrowGroupDataItem::updateActorSettings()
{
	const auto& s = m_setting;

	m_legendActors.update(iRIC::toStr(s.target), s.legendLength, s.standardValue, s.arrowSize, 15.0);
	m_legendActors.setLineWidth(s.lineWidth);
}
