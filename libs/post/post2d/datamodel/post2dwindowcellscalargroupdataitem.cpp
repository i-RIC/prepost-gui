#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcellscalargroupdataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowcellscalargroupdataitem_propertydialog.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainerutil.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post2dWindowCellScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &opacity, &colorMapSetting},
	colorMapSetting {},
	regionSetting {},
	opacity {"opacity", 50}
{}

Post2dWindowCellScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowCellScalarGroupDataItem::Setting& Post2dWindowCellScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowCellScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}

Post2dWindowCellScalarGroupDataItem::Post2dWindowCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	m_target {target},
	m_setting {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(mainWindow())},
	m_opacityToolBarWidget {new OpacityContainerWidget(mainWindow())}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	auto gType = topDataItem()->zoneDataItem()->dataContainer()->gridType();
	standardItem()->setText(gType->solutionCaption(target));

	m_colorMapToolBarWidget->hide();
	m_colorMapToolBarWidget->setSetting(&m_setting.colorMapSetting);
	connect(m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(m_colorMapToolBarWidget->modifiedSetting(), &m_setting.colorMapSetting);
		pushUpdateActorSettingCommand(com, this);
	});

	m_opacityToolBarWidget->hide();
	m_opacityToolBarWidget->setContainer(&m_setting.opacity);
	connect(m_opacityToolBarWidget, &OpacityContainerWidget::updated, [=](){
		auto com = m_opacityToolBarWidget->createModifyCommand();
		pushUpdateActorSettingCommand(com, this, false);
	});

	setupActors();
}

Post2dWindowCellScalarGroupDataItem::~Post2dWindowCellScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();
}

const std::string& Post2dWindowCellScalarGroupDataItem::target() const
{
	return m_target;
}

void Post2dWindowCellScalarGroupDataItem::updateActorSetting()
{
	auto cont = topDataItem()->zoneDataItem()->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOffWithEmptyPolyData();
		m_actor->SetMapper(mapper);
		mapper->Delete();
		return;
	}

	auto range = topDataItem()->zoneDataItem()->gridTypeDataItem()->cellValueRange(m_target);
	m_setting.colorMapSetting.setAutoValueRange(range);
	auto ps = cont->data()->data();

	auto filtered = m_setting.regionSetting.buildCellFilteredData(ps);
	filtered->GetCellData()->SetActiveScalars(m_target.c_str());
	auto mapper = m_setting.colorMapSetting.buildCellDataMapper(filtered, false);
	m_actor->SetMapper(mapper);

	filtered->Delete();
	mapper->Delete();

	m_setting.colorMapSetting.legend.imageSetting.apply(dataModel()->graphicsView());
	m_actor->GetProperty()->SetOpacity(m_setting.opacity);

	m_actor2DCollection->RemoveAllItems();
	if (m_setting.colorMapSetting.legend.visible) {
		m_actor2DCollection->AddItem(m_legendActor);
	}

	updateVisibilityWithoutRendering();
}

void Post2dWindowCellScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSetting();
}

void Post2dWindowCellScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowCellScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	m_actorCollection->AddItem(m_actor);

	m_setting.colorMapSetting.legend.imageSetting.setActor(m_legendActor);
	m_setting.colorMapSetting.legend.imageSetting.controller()->setItem(this);
	m_setting.colorMapSetting.legend.title = topDataItem()->zoneDataItem()->dataContainer()->gridType()->solutionCaption(target());

	updateActorSetting();
}

void Post2dWindowCellScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowCellScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowCellScalarGroupDataItem::update()
{
	updateActorSetting();
}

void Post2dWindowCellScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, scale, 1);
}

Post2dWindowCellScalarGroupTopDataItem* Post2dWindowCellScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowCellScalarGroupTopDataItem*> (parent());
}

void Post2dWindowCellScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowCellScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new PropertyDialog(this, p);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	return dialog;
}

bool Post2dWindowCellScalarGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (m_setting.opacity == 100) {return false;}

	return true;
}

void Post2dWindowCellScalarGroupDataItem::informSelection(VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleSelection(v);
	topDataItem()->zoneDataItem()->initCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleDeselection(v);
	topDataItem()->zoneDataItem()->clearCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	topDataItem()->zoneDataItem()->updateCellResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowCellScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post2dWindowCellScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	if (event->button() == Qt::LeftButton) {
		topDataItem()->zoneDataItem()->fixCellResultAttributeBrowser(event->pos(), v);
	}
}

void Post2dWindowCellScalarGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	m_setting.colorMapSetting.legend.imageSetting.controller()->handleResize(event, v);
}

void Post2dWindowCellScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = topDataItem()->zoneDataItem()->showAttributeBrowserActionForCellResult();
	menu->addAction(abAction);
}

bool Post2dWindowCellScalarGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_colorMapToolBarWidget->setParent(toolBar);
	m_colorMapToolBarWidget->show();

	toolBar->addWidget(m_colorMapToolBarWidget);
	toolBar->addSeparator();

	m_opacityToolBarWidget->setParent(toolBar);
	m_opacityToolBarWidget->show();

	toolBar->addWidget(m_opacityToolBarWidget);

	return true;
}

