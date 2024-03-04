#include "../../../datamodel/preprocessorgridtypedataitem.h"
#include "../gridbirdeyewindowdatamodel.h"
#include "../gridbirdeyewindowgraphicsview.h"
#include "gridbirdeyewindownodescalargroupdataitem.h"
#include "gridbirdeyewindownodescalargrouptopdataitem.h"
#include "gridbirdeyewindowzonedataitem.h"
#include "private/gridbirdeyewindownodescalargroupdataitem_impl.h"
#include "private/gridbirdeyewindownodescalargroupdataitem_settingeditwidget.h"

#include <guibase/graphicsmisc.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/grid/v4grid.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <vtkGeometryFilter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkWarpScalar.h>

GridBirdEyeWindowNodeScalarGroupDataItem::GridBirdEyeWindowNodeScalarGroupDataItem(const std::string& elevationTarget, GridBirdEyeWindowDataItem* p) :
	GridBirdEyeWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	impl {new Impl {elevationTarget, this}}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	impl->m_setting.colorMode = Setting::ColorMode::ByNodeScalar;
	impl->m_setting.colorTarget = elevationTarget.c_str();

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());

	auto gType = model->gridTypeDataItem()->gridType();
	QString caption;

	auto att1 = gType->gridAttribute(elevationTarget);
	if (att1 != nullptr) {
		caption = att1->caption();
	}
	auto att2 = gType->gridComplexAttribute(elevationTarget);
	if (att2 != nullptr) {
		caption = att2->caption();
	}

	standardItem()->setText(caption);

	for (const auto& pair : model->gridTypeDataItem()->colorMapSettings()) {
		const auto& name = pair.first;
		const auto caption = pair.second->valueCaption;
		auto cs = pair.second->copy();
		cs->legendSetting()->setVisible(true);
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

GridBirdEyeWindowNodeScalarGroupDataItem::~GridBirdEyeWindowNodeScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);
	r->RemoveActor2D(impl->m_legendActor);

	delete impl;
}

const std::string& GridBirdEyeWindowNodeScalarGroupDataItem::elevationTarget() const
{
	return impl->m_elevationTarget;
}

void GridBirdEyeWindowNodeScalarGroupDataItem::updateActorSetting()
{
	impl->m_legendActor->VisibilityOff();
	m_actor2DCollection->RemoveAllItems();

	auto grid = topDataItem()->zoneDataItem()->grid();
	if (grid == nullptr) {return;}

	auto filtered = impl->m_setting.regionSetting.buildNodeFilteredData(grid->grid()->vtkData()->data());
	filtered->GetPointData()->SetActiveScalars(impl->m_elevationTarget.c_str());

	auto warp = vtkSmartPointer<vtkWarpScalar>::New();
	warp->UseNormalOn();
	warp->SetNormal(0, 0, 1);
	warp->SetScaleFactor(1);
	warp->SetInputData(filtered);

	auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
	filter->SetInputConnection(warp->GetOutputPort());
	filter->Update();

	filtered->Delete();
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
			auto mapper = cs->buildPointDataMapper(data);
			impl->m_actor->SetMapper(mapper);
			mapper->Delete();
			cs->legendSetting()->imgSetting()->setActor(impl->m_legendActor);
			cs->legendSetting()->imgSetting()->controller()->setItem(this);
			auto v = dataModel()->graphicsView();
			cs->legendSetting()->imgSetting()->apply(v);
		}
	} else if (impl->m_setting.colorMode == Setting::ColorMode::ByCellScalar) {
		auto cs = activeColorMapSetting();
		if (cs != nullptr) {
			impl->m_colorMapToolBarWidget->setSetting(cs);
			data->GetCellData()->SetActiveScalars(iRIC::toStr(impl->m_setting.colorTarget).c_str());
			auto mapper = cs->buildCellDataMapper(data, false);
			impl->m_actor->SetMapper(mapper);
			mapper->Delete();
			cs->legendSetting()->imgSetting()->setActor(impl->m_legendActor);
			cs->legendSetting()->imgSetting()->controller()->setItem(this);
			auto v = dataModel()->graphicsView();
			cs->legendSetting()->imgSetting()->apply(v);
		}
	}

	impl->m_actor->GetProperty()->SetOpacity(impl->m_setting.opacity);
	updateVisibilityWithoutRendering();
}

void GridBirdEyeWindowNodeScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_actor);
	r->AddActor2D(impl->m_legendActor);

	m_actorCollection->AddItem(impl->m_actor);

	updateActorSetting();
}

void GridBirdEyeWindowNodeScalarGroupDataItem::update()
{
	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());

	for (const auto& pair : model->gridTypeDataItem()->colorMapSettings()) {
		const auto& name = pair.first;
		auto cs = pair.second;

		impl->m_colorMapSettings.at(name)->setAutoValueRange(cs->autoMinValue, cs->autoMaxValue);
	}

	updateActorSetting();
}

void GridBirdEyeWindowNodeScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GridBirdEyeWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	dialog->resize(900, 700);
	return dialog;
}

void GridBirdEyeWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	impl->m_actor->SetScale(1, 1, scale);
}

void GridBirdEyeWindowNodeScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
	}
}

void GridBirdEyeWindowNodeScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
	}
}

void GridBirdEyeWindowNodeScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
	}
}

bool GridBirdEyeWindowNodeScalarGroupDataItem::addToolBarButtons(QToolBar* toolBar)
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

void GridBirdEyeWindowNodeScalarGroupDataItem::gatherActiveColorMapLegends(std::vector<ColorMapLegendSettingContainerI*>* legends)
{
	if (! (isAncientChecked() && isChecked())) {return;}
	if (impl->m_setting.colorMode == Setting::ColorMode::Custom) {return;}

	auto cs = activeColorMapSetting();
	if (cs == nullptr) {return;}

	legends->push_back(cs->legendSetting());
}

void GridBirdEyeWindowNodeScalarGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleResize(event, v);
	}
}

void GridBirdEyeWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void GridBirdEyeWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);

	for (const auto& pair : impl->m_colorMapSettings) {
		writer.writeStartElement("ColorMapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
}

GridBirdEyeWindowNodeScalarGroupTopDataItem* GridBirdEyeWindowNodeScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<GridBirdEyeWindowNodeScalarGroupTopDataItem*> (parent());
}

ColorMapSettingContainerI* GridBirdEyeWindowNodeScalarGroupDataItem::colorMapSetting(const std::string& name) const
{
	auto it = impl->m_colorMapSettings.find(name);
	if (it == impl->m_colorMapSettings.end()) {return nullptr;}

	return it->second;
}

ColorMapSettingContainerI* GridBirdEyeWindowNodeScalarGroupDataItem::activeColorMapSetting() const
{
	if (impl->m_setting.colorMode == Setting::ColorMode::Custom) {return nullptr;}

	return colorMapSetting(iRIC::toStr(impl->m_setting.colorTarget));
}
