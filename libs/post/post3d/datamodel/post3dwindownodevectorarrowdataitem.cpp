#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodevectorarrowdataitem.h"
#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "../post3dwindowgraphicsview.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QDomElement>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkCamera.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

Post3dWindowNodeVectorArrowDataItem::Post3dWindowNodeVectorArrowDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.png"), p},
	m_extractGrid {vtkExtractGrid::New()},
	m_transformFilter {vtkTransformFilter::New()}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor(m_arrowsActor.actor());

	updateActorSettings();
}

Post3dWindowNodeVectorArrowDataItem::~Post3dWindowNodeVectorArrowDataItem()
{
	renderer()->RemoveActor(m_arrowsActor.actor());
	m_extractGrid->Delete();
	m_transformFilter->Delete();
}

Post3dWindowNodeVectorArrowSettingDialog::FaceSetting Post3dWindowNodeVectorArrowDataItem::setting() const
{
	Post3dWindowNodeVectorArrowSettingDialog::FaceSetting ret;

	ret.enabled = (standardItem()->checkState() == Qt::Checked);
	ret.caption = standardItem()->text();
	ret.arrowSetting = m_arrowSetting;
	ret.faceSetting = m_faceSetting;
	ret.samplingRateSetting = m_samplingRateSetting;

	return ret;
}

void Post3dWindowNodeVectorArrowDataItem::setSetting(const Post3dWindowNodeVectorArrowSettingDialog::FaceSetting& faceSetting)
{
	m_isCommandExecuting = true;

	standardItem()->setText(faceSetting.caption);
	standardItem()->setCheckState(Qt::Checked);

	m_arrowSetting = faceSetting.arrowSetting;
	m_faceSetting = faceSetting.faceSetting;
	m_samplingRateSetting = faceSetting.samplingRateSetting;

	updateActorSettings();

	m_isCommandExecuting = false;
}

void Post3dWindowNodeVectorArrowDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowNodeVectorArrowDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_standardItem->setText(elem.attribute("caption", tr("Face")));

	m_arrowSetting.load(node);
	m_faceSetting.load(node);
	m_samplingRateSetting.load(node);

	updateActorSettings();
}

void Post3dWindowNodeVectorArrowDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("caption", m_standardItem->text());

	m_arrowSetting.save(writer);
	m_faceSetting.save(writer);
	m_samplingRateSetting.save(writer);

	updateActorSettings();
}

void Post3dWindowNodeVectorArrowDataItem::innerUpdateZScale(double scale)
{
	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->Scale(1, 1, scale);
	m_transformFilter->SetTransform(transform);
	updatePolyData();
}

void Post3dWindowNodeVectorArrowDataItem::innerUpdate2Ds()
{
	vtkCamera* cam = renderer()->GetActiveCamera();
	double scale = cam->GetParallelScale();
	if (scale != m_arrowSetting.oldCameraScale) {
		updatePolyData();
	}
	m_arrowSetting.oldCameraScale = scale;
}


void Post3dWindowNodeVectorArrowDataItem::updateActorSettings()
{
	m_arrowsActor.actor()->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	const auto& setting = dynamic_cast<Post3dWindowNodeVectorArrowGroupDataItem*> (parent())->setting();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent()->parent())->dataContainer();
	if (cont == nullptr) {return;}

	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}

	if (setting.target == "") {return;}

	updatePolyData();
	updateColorSetting();

	m_actorCollection->AddItem(m_arrowsActor.actor());

	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeVectorArrowDataItem::updatePolyData()
{
	const auto& setting = dynamic_cast<Post3dWindowNodeVectorArrowGroupDataItem*> (parent())->setting();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent()->parent())->dataContainer();
	if (cont == nullptr) {return;}

	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}

	if (setting.target == "") {return;}

	vtkPointData* pd = ps->GetPointData();
	pd->SetActiveVectors(iRIC::toStr(setting.target).c_str());
	m_arrowsActor.setScaleFactor(setting.scaleFactor(dataModel()->graphicsView()->stdDistance(1)));

	const auto& fS = m_faceSetting;
	m_extractGrid->SetVOI(fS.iMin, fS.iMax, fS.jMin, fS.jMax, fS.kMin, fS.kMax);

	const auto& srSetting = m_samplingRateSetting;
	m_extractGrid->SetSampleRate(srSetting.iSamplingRate, srSetting.jSamplingRate, srSetting.kSamplingRate);

	m_extractGrid->SetInputData(ps);
	m_extractGrid->Update();

	double height = dataModel()->graphicsView()->stdDistance(m_arrowSetting.arrowSize);
	m_arrowsActor.setConeHeight(height);

	vtkPolyData* filteredData = setting.buildFilteredData(m_extractGrid->GetOutput());
	m_arrowsActor.setPolyData(filteredData);
	filteredData->Delete();

	m_arrowsActor.setLineWidth(m_arrowSetting.lineWidth);
}

void Post3dWindowNodeVectorArrowDataItem::updateColorSetting()
{
	Post3dWindowGridTypeDataItem* typedi = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent()->parent());
	const auto& s = m_arrowSetting;
	switch (s.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		m_arrowsActor.setColorModeToCustom(s.customColor);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		LookupTableContainer* stc = typedi->nodeLookupTable(s.colorTarget);
		m_arrowsActor.setColorModeToValue(iRIC::toStr(s.colorTarget), stc->vtkObj());
		break;
	}
}
