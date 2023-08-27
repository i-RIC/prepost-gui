#include "measureddatafiledataitem.h"
#include "measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatavectordataitem.h"
#include "measureddatavectorgroupdataitem.h"
#include "private/measureddatavectorgroupdataitem_impl.h"
#include "private/measureddatavectorgroupdataitem_settingeditwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QIcon>
#include <QSettings>

#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkGeometryFilter.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <cmath>
#include <vector>

MeasuredDataVectorGroupDataItem::Impl::Impl(MeasuredDataVectorGroupDataItem* item) :
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_item {item}
{}

MeasuredDataVectorGroupDataItem::Impl::~Impl()
{
	vtkRenderer* r = m_item->renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();
}

void MeasuredDataVectorGroupDataItem::Impl::setupActors()
{
	vtkRenderer* r = m_item->renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	m_actor->GetProperty()->SetLighting(false);
}

void MeasuredDataVectorGroupDataItem::Impl::updateCheckState()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.arrowsSetting.target, m_item->m_childItems, true);
}

// public interfaces

MeasuredDataVectorGroupDataItem::MeasuredDataVectorGroupDataItem(GraphicsWindowDataItem* p) :
	GraphicsWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	impl->setupActors();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();

	for (const auto& name : md->vectorNames()) {
		auto item = new MeasuredDataVectorDataItem(name, name.c_str(), this);
		m_childItems.push_back(item);
	}

	impl->m_setting.arrowsSetting.legend.imageSetting.setActor(impl->m_legendActor);
	impl->m_setting.arrowsSetting.legend.imageSetting.controller()->setItem(this);

	if (md->scalarNames().size() > 0) {
		auto name = md->scalarNames().at(0);
		impl->m_setting.arrowsSetting.colorTarget = name.c_str();
	}

	if (md->vectorNames().size() > 0) {
		auto name = md->vectorNames().at(0);
		setTarget(name);
	}
}

MeasuredDataVectorGroupDataItem::~MeasuredDataVectorGroupDataItem()
{
	delete impl;
}

void MeasuredDataVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	NamedGraphicsWindowDataItemTool::checkItemWithName(iRIC::toStr(impl->m_setting.arrowsSetting.target), m_childItems);
}

void MeasuredDataVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

std::string MeasuredDataVectorGroupDataItem::target() const
{
	return impl->m_setting.arrowsSetting.target;
}

void MeasuredDataVectorGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	impl->m_setting.arrowsSetting.target = target.c_str();
	impl->m_setting.arrowsSetting.legend.title = target.c_str();

	updateActorSetting();
}

void MeasuredDataVectorGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void MeasuredDataVectorGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Arrow Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void MeasuredDataVectorGroupDataItem::informGridUpdate()
{
	updateActorSetting();
}

void MeasuredDataVectorGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void MeasuredDataVectorGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.max());
}

void MeasuredDataVectorGroupDataItem::update()
{
	informGridUpdate();
}

void MeasuredDataVectorGroupDataItem::innerUpdate2Ds()
{
	updateActorSetting();
}

ColorMapSettingContainerI* MeasuredDataVectorGroupDataItem::colorMapSetting(const std::string& target)
{
	auto pItem = dynamic_cast <MeasuredDataFileDataItem*>(parent())->pointGroupDataItem();
	return pItem->colorMapSetting(target);
}

QDialog* MeasuredDataVectorGroupDataItem::propertyDialog(QWidget* p)
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {
		return nullptr;
	}

	if (md->vectorNames().size() == 0) {
		return nullptr;
	}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Arrows Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void MeasuredDataVectorGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto& as = impl->m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleResize(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

void MeasuredDataVectorGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = impl->m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	if (as.legend.imageSetting.controller()->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {return;}

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
}

void MeasuredDataVectorGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = impl->m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMousePressEvent(event, v);
	if (as.legend.imageSetting.controller()->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {return;}

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void MeasuredDataVectorGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = impl->m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	if (as.legend.imageSetting.controller()->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {return;}

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
}

void MeasuredDataVectorGroupDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	updateActorSetting();
}

void MeasuredDataVectorGroupDataItem::updateActorSetting()
{
	impl->m_actor->VisibilityOff();
	impl->m_legendActor->VisibilityOff();
	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {return;}
	if (impl->m_setting.arrowsSetting.target == "") {return;}

	impl->m_setting.arrowsSetting.updateStandardValueIfNeeded(md->pointData()->GetPointData());
	auto filteredData = impl->m_setting.arrowsSetting.buildFilteredData(md->pointData());

	auto sampledData = impl->m_setting.filteringSetting.buildSampledData(filteredData);
	filteredData->Delete();

	auto view = dataModel()->graphicsView();
	auto arrowsData = impl->m_setting.arrowsSetting.buildArrowsPolygonData(sampledData, view);
	sampledData->Delete();

	if (impl->m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputData(arrowsData);
		mapper->SetInputConnection(gfilter->GetOutputPort());
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();

		impl->m_actor->GetProperty()->SetColor(impl->m_setting.arrowsSetting.customColor);
	} else if (impl->m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		arrowsData->GetPointData()->SetActiveScalars(iRIC::toStr(impl->m_setting.arrowsSetting.colorTarget).c_str());
		auto cs = colorMapSetting(iRIC::toStr(impl->m_setting.arrowsSetting.colorTarget));
		if (cs == nullptr) {return;}
		auto mapper = cs->buildPointDataMapper(arrowsData);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
	}
	arrowsData->Delete();

	auto v = dataModel()->graphicsView();
	impl->m_actor->GetProperty()->SetLineWidth(impl->m_setting.arrowsSetting.lineWidth * v->devicePixelRatioF());

	actorCollection()->AddItem(impl->m_actor);
	actor2DCollection()->AddItem(impl->m_legendActor);
	updateVisibilityWithoutRendering();

	impl->m_setting.arrowsSetting.legend.imageSetting.apply(v);

	auto& as = impl->m_setting.arrowsSetting;
	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->apply(v);
}
