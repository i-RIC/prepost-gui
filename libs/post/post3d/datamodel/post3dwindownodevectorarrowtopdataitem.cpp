#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindownodevectorarrowtopdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "post3dwindownodevectorarrowsettingdialog.h"
#include "private/post3dwindownodevectorarrowtopdataitem_createcommand.h"

#include <guibase/objectbrowserview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QDomNodeList>
#include <QMessageBox>
#include <QMenu>
#include <QXmlStreamWriter>

Post3dWindowNodeVectorArrowTopDataItem::Post3dWindowNodeVectorArrowTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

double Post3dWindowNodeVectorArrowTopDataItem::zScale() const
{
	return m_zScale;
}

void Post3dWindowNodeVectorArrowTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowNodeVectorArrowTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowNodeVectorArrowGroupDataItem*> (item);
		child->update();
	}
}

void Post3dWindowNodeVectorArrowTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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
		} else if (childElem.nodeName() == "ArrowGroup") {
			auto item = new Post3dWindowNodeVectorArrowGroupDataItem(this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(children.at(i));
			m_childItems.push_back(item);
		}
	}
}
void Post3dWindowNodeVectorArrowTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

	// arrows
	for (auto item : m_childItems) {
		writer.writeStartElement("ArrowGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowNodeVectorArrowTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

QDialog* Post3dWindowNodeVectorArrowTopDataItem::addDialog(QWidget* p)
{
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return nullptr;
	}

	if (childItems().size() >= 4) {
		QMessageBox::warning(postProcessorWindow(), tr("Warning"), tr("A maximum of four arrows may be defined."));
		return nullptr;
	}

	auto dialog = new Post3dWindowNodeVectorArrowSettingDialog(p);
	dialog->setZoneData(zoneData);

	ArrowSettingContainer arrowSetting;
	dialog->setArrowSetting(arrowSetting);

	std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting> faceSettings;
	dialog->setFaceSettings(faceSettings);

	return dialog;
}

void Post3dWindowNodeVectorArrowTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<Post3dWindowNodeVectorArrowSettingDialog*> (propDialog);
	auto arrowSetting = dialog->arrowSetting();
	auto faceSettings = dialog->faceSettings();

	pushRenderCommand(new CreateCommand(arrowSetting, faceSettings, this), this);
}
