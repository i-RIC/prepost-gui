#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodevectorarrowdataitem.h"
#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <misc/stringtool.h>

#include <vtkActor.h>
#include <vtkTransformFilter.h>

Post3dWindowNodeVectorArrowDataItem::Setting::Setting() :
	CompositeContainer({&face, &filtering, &arrow})
{}

Post3dWindowNodeVectorArrowDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post3dWindowNodeVectorArrowDataItem::Setting& Post3dWindowNodeVectorArrowDataItem::Setting::operator=(const Setting& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post3dWindowNodeVectorArrowDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&> (c));
}

Post3dWindowNodeVectorArrowDataItem::Post3dWindowNodeVectorArrowDataItem(const QString& label, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	m_actor {vtkActor::New()},
	m_transformFilter {vtkTransformFilter::New()},
	m_setting {},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, Deletable);
	m_standardItem->setText(label);

	setupActors();
}

Post3dWindowNodeVectorArrowDataItem::~Post3dWindowNodeVectorArrowDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);

	m_actor->Delete();
	m_transformFilter->Delete();
}

vtkStructuredGrid* Post3dWindowNodeVectorArrowDataItem::faceGrid() const
{
	auto data = groupDataItem()->data()->data();
	if (data == nullptr) {return nullptr;}

	return m_setting.face.extractFace(vtkStructuredGrid::SafeDownCast(data->data()));
}

const Post3dWindowNodeVectorArrowDataItem::Setting& Post3dWindowNodeVectorArrowDataItem::setting() const
{
	return m_setting;
}

void Post3dWindowNodeVectorArrowDataItem::setSetting(const Setting& setting)
{
	m_setting = setting;
}

void Post3dWindowNodeVectorArrowDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowNodeVectorArrowDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	updateActorSettings();
}

void Post3dWindowNodeVectorArrowDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post3dWindowNodeVectorArrowDataItem::innerUpdateZScale(double zscale)
{
	m_zScale = zscale;

	auto t = vtkSmartPointer<vtkTransform>::New();
	t->Scale(1, 1, zscale);
	m_transformFilter->SetTransform(t);

	updateActorSettings();
}

void Post3dWindowNodeVectorArrowDataItem::innerUpdate2Ds()
{
	updateActorSettings();
}

void Post3dWindowNodeVectorArrowDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);

	auto t = vtkSmartPointer<vtkTransform>::New();
	m_transformFilter->SetTransform(t);
}

void Post3dWindowNodeVectorArrowDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto data = groupDataItem()->data()->data();
	if (data == nullptr) {return;}

	auto grid = vtkStructuredGrid::SafeDownCast(data->data());

	auto grid2 = m_setting.face.extractFace(grid);

	auto sampledData = m_setting.filtering.buildSampledData(grid2);
	grid2->Delete();

	auto filteredData = groupDataItem()->m_setting.buildFilteredData(sampledData);
	sampledData->Delete();

	m_transformFilter->SetInputData(filteredData);
	m_transformFilter->Update();
	filteredData->Delete();

	const auto& groupSetting = groupDataItem()->m_setting;
	m_setting.arrow.target = groupSetting.target;
	m_setting.arrow.standardValue = groupSetting.standardValue;
	m_setting.arrow.legendLength = groupSetting.legendLength;
	m_setting.arrow.minimumValue = groupSetting.minimumValue;

	auto v = dataModel()->graphicsView();
	auto arrowsData = m_setting.arrow.buildArrowsPolygonData(m_transformFilter->GetOutput(), v, m_zScale);

	if (m_setting.arrow.colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputData(arrowsData);
		mapper->SetInputConnection(gfilter->GetOutputPort());
		m_actor->SetMapper(mapper);
		mapper->Delete();
		m_actor->GetProperty()->SetColor(m_setting.arrow.customColor);
	} else if (m_setting.arrow.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto colorTarget = iRIC::toStr(m_setting.arrow.colorTarget);
		arrowsData->GetPointData()->SetActiveScalars(colorTarget.c_str());
		auto cs = groupDataItem()->colorMapSetting(colorTarget);
		if (cs == nullptr) {return;}
		auto mapper = cs->buildPointDataMapper(arrowsData);
		m_actor->SetMapper(mapper);
		mapper->Delete();
	}
	arrowsData->Delete();
	m_actor->GetProperty()->SetLineWidth(m_setting.arrow.lineWidth * v->devicePixelRatioF());

	actorCollection()->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

Post3dWindowNodeVectorArrowGroupDataItem* Post3dWindowNodeVectorArrowDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowNodeVectorArrowGroupDataItem*> (parent());
}
