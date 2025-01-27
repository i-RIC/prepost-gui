#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindownodevectorarrowgrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/qscreenutil.h>

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
	dialog->resize(QScreenUtil::sizeWithinScreen(ColorMapSettingEditWidgetI::standardDialogSize(100)));

	return dialog;
}

vtkPointSet* Post2dWindowNodeVectorArrowGroupStructuredDataItem::buildFilteredData()
{
	auto dc = topDataItem()->zoneDataItem()->v4DataContainer();
	if (dc == nullptr) {return nullptr;}
	auto data = dc->gridData();
	if (data == nullptr) {return nullptr;}
	auto data2 = data->grid()->vtkData()->data();
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

	doUpdateActorSetting();
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
	m_regionSetting.save(writer);
	m_filteringSetting.save(writer);

	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);
}
