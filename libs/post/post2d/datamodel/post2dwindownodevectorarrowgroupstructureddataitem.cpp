#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupstructureddataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Post2dWindowNodeVectorArrowGroupStructuredDataItem(Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem {p}
{}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowNodeVectorArrowGroupStructuredDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

vtkPointSet* Post2dWindowNodeVectorArrowGroupStructuredDataItem::buildFilteredData()
{
	auto dc = zoneDataItem()->dataContainer();
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

	updateCheckState();
	updateActorSetting();
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
	m_regionSetting.save(writer);
	m_filteringSetting.save(writer);

	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);
}
