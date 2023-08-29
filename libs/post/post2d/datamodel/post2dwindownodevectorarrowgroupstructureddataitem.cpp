#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindownodevectorarrowgrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Post2dWindowNodeVectorArrowGroupStructuredDataItem(const std::string& name, Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem {name, p}
{}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowNodeVectorArrowGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Arrows Setting (%1)").arg(m_standardItem->text()));
	dialog->resize(900, 650);

	return dialog;
}

vtkPointSet* Post2dWindowNodeVectorArrowGroupStructuredDataItem::buildFilteredData()
{
	auto dc = topDataItem()->zoneDataItem()->dataContainer();
	if (dc == nullptr) {return nullptr;}
	auto data = dc->data();
	if (data == nullptr) {return nullptr;}
	auto data2 = data->data();
	auto clippedData = m_regionSetting.buildNodeFilteredData(data2);

	auto structured = vtkStructuredGrid::SafeDownCast(clippedData);
	if (structured == nullptr) {
		m_setting.updateStandardValueIfNeeded(clippedData->GetPointData());
		auto filteredData = m_setting.buildFilteredData(clippedData);
		clippedData->Delete();

		return filteredData;
	}

	auto filtered = m_filteringSetting.buildSampledData(structured);
	clippedData->Delete();

	m_setting.updateStandardValueIfNeeded(filtered->GetPointData());
	auto filtered2 = m_setting.buildFilteredData(filtered);
	filtered->Delete();

	return filtered2;
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	m_regionSetting.load(node);
	m_filteringSetting.load(node);

	Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(node);

	updateActorSetting();
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
	m_regionSetting.save(writer);
	m_filteringSetting.save(writer);

	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);
}
