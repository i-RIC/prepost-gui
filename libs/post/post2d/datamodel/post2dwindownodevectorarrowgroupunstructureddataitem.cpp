#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(Post2dWindowDataItem* p) :
	Post2dWindowNodeVectorArrowGroupDataItem(p)
{}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

vtkPointSet* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::buildFilteredData()
{
	auto data = zoneDataItem()->dataContainer()->data()->data();
	auto clippedData = m_regionSetting.buildNodeFilteredData(data);

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

	updateCheckState();
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
	m_regionSetting.save(writer);
	m_filteringSetting.save(writer);

	Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(writer);
}
