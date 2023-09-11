#include "post2dwindownodevectorarrowgrouptopdataitem.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupunstructureddataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(const std::string& name, Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem(name, p)
{}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Arrows Setting (%1)").arg(m_standardItem->text()));
	dialog->resize(900, 650);

	return dialog;
}

vtkPointSet* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::buildFilteredData()
{
	auto dc = topDataItem()->zoneDataItem()->dataContainer();
	if (dc == nullptr) {return nullptr;}
	auto data = dc->data();
	if (data == nullptr) {return nullptr;}

	auto data2 = data->data();
	auto clippedData = m_regionSetting.buildNodeFilteredData(data2);

	m_setting.updateStandardValueIfNeeded(clippedData->GetPointData());
	auto filtered = m_setting.buildFilteredData(clippedData);
	clippedData->Delete();

	auto sampled = m_filteringSetting.buildSampledData(filtered);
	filtered->Delete();

	return sampled;
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	m_regionSetting.load(node);
	m_filteringSetting.load(node);

	Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(node);

	updateActorSetting();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
	m_regionSetting.save(writer);
	m_filteringSetting.save(writer);

	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);
}
