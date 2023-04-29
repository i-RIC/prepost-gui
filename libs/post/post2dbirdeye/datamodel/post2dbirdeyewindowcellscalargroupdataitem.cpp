#include "../post2dbirdeyewindowdatamodel.h"
#include "../post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowcellscalargroupdataitem.h"
#include "post2dbirdeyewindowcellscalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include "private/post2dbirdeyewindowcellscalargroupdataitem_impl.h"
#include "private/post2dbirdeyewindowcellscalargroupdataitem_settingeditwidget.h"

#include <guibase/graphicsmisc.h>
#include <guibase/vtktool/vtkcelldatawarp.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <vtkGeometryFilter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkWarpScalar.h>

Post2dBirdEyeWindowCellScalarGroupDataItem::Post2dBirdEyeWindowCellScalarGroupDataItem(const std::string& elevationTarget, Post2dBirdEyeWindowDataItem* p) :
	Post2dBirdEyeWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	impl {new Impl {elevationTarget, this}}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	impl->m_setting.colorMode = Setting::ColorMode::ByCellScalar;
	impl->m_setting.colorTarget = elevationTarget.c_str();

	auto gType = topDataItem()->zoneDataItem()->dataContainer()->gridType();
	standardItem()->setText(gType->solutionCaption(elevationTarget));

	auto cont = topDataItem()->zoneDataItem()->dataContainer();
	for (const auto& pair : cont->data()->valueRangeSet().pointDataValueRanges()) {
		const auto& name = pair.first;
		auto cs = new ColorMapSettingContainer();
		auto caption = gType->output(name)->caption();
		cs->valueCaption = caption;
		cs->legend.title = caption;
		cs->setAutoValueRange(pair.second);
		impl->m_colorMapSettings.insert({name, cs});
	}
	for (const auto& pair : cont->data()->valueRangeSet().cellDataValueRanges()) {
		const auto& name = pair.first;
		auto cs = new ColorMapSettingContainer();
		auto caption = gType->output(name)->caption();
		cs->valueCaption = caption;
		cs->legend.title = caption;
		cs->setAutoValueRange(pair.second);
		impl->m_colorMapSettings.insert({name, cs});
	}

	impl->m_colorMapToolBarWidget->hide();
	connect(impl->m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto cs = impl->m_colorMapSettings.at(iRIC::toStr(impl->m_setting.colorTarget));
		auto com = new ColorMapSettingModifyCommand(impl->m_colorMapToolBarWidget->modifiedSetting(), cs);
		pushUpdateActorSettingCommand(com, this);
	});

	impl->m_opacityToolBarWidget->hide();
	impl->m_opacityToolBarWidget->setContainer(&impl->m_setting.opacity);
	connect(impl->m_opacityToolBarWidget, &OpacityContainerWidget::updated, [=](){
		auto com = impl->m_opacityToolBarWidget->createModifyCommand();
		pushUpdateActorSettingCommand(com, this, false);
	});

	setupActors();
}

Post2dBirdEyeWindowCellScalarGroupDataItem::~Post2dBirdEyeWindowCellScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);
	r->RemoveActor2D(impl->m_legendActor);

	delete impl;
}

const std::string& Post2dBirdEyeWindowCellScalarGroupDataItem::elevationTarget() const
{
	return impl->m_elevationTarget;
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::updateActorSetting()
{
	impl->m_legendActor->VisibilityOff();
	m_actor2DCollection->RemoveAllItems();

	auto cont = topDataItem()->zoneDataItem()->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}

	auto filtered = impl->m_setting.regionSetting.buildNodeFilteredData(cont->data()->data());
	filtered->GetCellData()->SetActiveScalars(impl->m_elevationTarget.c_str());

	auto warped = vtkCellDataWarp::warp(filtered);
	filtered->Delete();

	auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
	filter->SetInputData(warped);
	filter->Update();
	warped->Delete();

	auto data = filter->GetOutput();

	impl->m_colorMapToolBarWidget->setEnabled(true);

	if (impl->m_setting.colorMode == Setting::ColorMode::Custom) {
		impl->m_colorMapToolBarWidget->setDisabled(true);

		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(data);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
		impl->m_actor->GetProperty()->SetColor(impl->m_setting.customColor);
	} else if (impl->m_setting.colorMode == Setting::ColorMode::ByNodeScalar) {
		auto cs = activeColorMapSetting();
		if (cs != nullptr) {
			impl->m_colorMapToolBarWidget->setSetting(cs);
			data->GetPointData()->SetActiveScalars(iRIC::toStr(impl->m_setting.colorTarget).c_str());
			impl->m_setting.contourSetting.setColorMapSetting(cs);
			auto data2 = impl->m_setting.contourSetting.buildFilteredData(data);
			auto mapper = cs->buildPointDataMapper(data2);
			data2->Delete();
			impl->m_actor->SetMapper(mapper);
			mapper->Delete();
			cs->legend.imageSetting.setActor(impl->m_legendActor);
			cs->legend.imageSetting.controller()->setItem(this);
			auto v = dataModel()->graphicsView();
			cs->legend.imageSetting.apply(v);
		}
	} else if (impl->m_setting.colorMode == Setting::ColorMode::ByCellScalar) {
		auto cs = activeColorMapSetting();
		if (cs != nullptr) {
			impl->m_colorMapToolBarWidget->setSetting(cs);
			data->GetPointData()->SetActiveScalars(iRIC::toStr(impl->m_setting.colorTarget).c_str());
			auto mapper = cs->buildPointDataMapper(data);
			impl->m_actor->SetMapper(mapper);
			mapper->Delete();
			cs->legend.imageSetting.setActor(impl->m_legendActor);
			cs->legend.imageSetting.controller()->setItem(this);
			auto v = dataModel()->graphicsView();
			cs->legend.imageSetting.apply(v);
		}
	}

	impl->m_actor->GetProperty()->SetOpacity(impl->m_setting.opacity);

	auto v = dataModel()->graphicsView();
	impl->m_actor->GetProperty()->SetLineWidth(impl->m_setting.contourSetting.contourLineWidth * v->devicePixelRatioF());

	updateVisibilityWithoutRendering();
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_actor);
	r->AddActor2D(impl->m_legendActor);

	m_actorCollection->AddItem(impl->m_actor);

	updateActorSetting();
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::update()
{
	auto cont = topDataItem()->zoneDataItem()->dataContainer();
	for (const auto& pair : cont->data()->valueRangeSet().pointDataValueRanges()) {
		impl->m_colorMapSettings.at(pair.first)->setAutoValueRange(pair.second);
	}
	for (const auto& pair : cont->data()->valueRangeSet().cellDataValueRanges()) {
		impl->m_colorMapSettings.at(pair.first)->setAutoValueRange(pair.second);
	}

	updateActorSetting();
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dBirdEyeWindowCellScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	dialog->resize(900, 700);
	return dialog;
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::innerUpdateZScale(double scale)
{
	impl->m_actor->SetScale(1, 1, scale);
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::informSelection(VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleSelection(v);
	}
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleDeselection(v);
	}
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	}
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMousePressEvent(event, v);
	}
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	}
}

bool Post2dBirdEyeWindowCellScalarGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
		impl->m_colorMapToolBarWidget->setParent(toolBar);
		impl->m_colorMapToolBarWidget->show();

		toolBar->addWidget(impl->m_colorMapToolBarWidget);
		toolBar->addSeparator();

		impl->m_opacityToolBarWidget->setParent(toolBar);
		impl->m_opacityToolBarWidget->show();

		toolBar->addWidget(impl->m_opacityToolBarWidget);

		return true;
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleResize(event, v);
	}
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		if (child.nodeName() == "ColorMapSetting") {
			auto elem = child.toElement();
			auto name = iRIC::toStr(elem.attribute("name"));
			auto it = impl->m_colorMapSettings.find(name);
			if (it == impl->m_colorMapSettings.end()) {continue;}

			it->second->load(child);
		}
	}

	updateActorSetting();
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);

	for (const auto& pair : impl->m_colorMapSettings) {
		writer.writeStartElement("ColorMapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
}

Post2dBirdEyeWindowCellScalarGroupTopDataItem* Post2dBirdEyeWindowCellScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowCellScalarGroupTopDataItem*> (parent());
}

ColorMapSettingContainer* Post2dBirdEyeWindowCellScalarGroupDataItem::colorMapSetting(const std::string& name) const
{
	auto it = impl->m_colorMapSettings.find(name);
	if (it == impl->m_colorMapSettings.end()) {return nullptr;}

	return it->second;
}

ColorMapSettingContainer* Post2dBirdEyeWindowCellScalarGroupDataItem::activeColorMapSetting() const
{
	if (impl->m_setting.colorMode == Setting::ColorMode::Custom) {return nullptr;}

	return colorMapSetting(iRIC::toStr(impl->m_setting.colorTarget));
}
