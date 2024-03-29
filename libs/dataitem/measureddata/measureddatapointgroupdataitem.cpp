#include "measureddatafiledataitem.h"
#include "measureddatapointdataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "private/measureddatapointgroupdataitem_impl.h"
#include "private/measureddatapointgroupdataitem_settingeditwidget.h"
#include "private/measureddatapointgroupdataitem_toolbarwidget.h"
#include "private/measureddatapointgroupdataitem_toolbarwidgetcontroller.h"

#include <guibase/graphicsmisc.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtkCustomScalarBarActor.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/project/measured/measureddata.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QMainWindow>
#include <QStandardItem>
#include <QToolBar>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

MeasuredDataPointGroupDataItem::Impl::Impl(MeasuredDataPointGroupDataItem* item) :
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_toolBarWidget {new ToolBarWidget {m_setting, item, item->mainWindow()}},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(item->mainWindow())},
	m_toolBarWidgetController {new ToolBarWidgetController(m_colorMapToolBarWidget, item)},
	m_item {item}
{}

MeasuredDataPointGroupDataItem::Impl::~Impl()
{
	vtkRenderer* r = m_item->renderer();

	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();
}

void MeasuredDataPointGroupDataItem::Impl::setupActors()
{
	vtkRenderer* r = m_item->renderer();

	m_actor->GetProperty()->SetLighting(false);
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);
}

void MeasuredDataPointGroupDataItem::Impl::updateCheckState()
{
	if (m_setting.mappingMode == Setting::MappingMode::Arbitrary) {
		NamedGraphicsWindowDataItemTool::checkItemWithName("", m_item->m_childItems, true);
	} else {
		NamedGraphicsWindowDataItemTool::checkItemWithName(iRIC::toStr(m_setting.value), m_item->m_childItems, true);
	}
}

// public interfaces

MeasuredDataPointGroupDataItem::MeasuredDataPointGroupDataItem(GraphicsWindowDataItem* p) :
	GraphicsWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	impl->setupActors();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(p)->measuredData();

	for (int i = 0; i < md->scalarNames().size(); ++i) {
		std::string name = md->scalarNames().at(i);
		MeasuredDataPointDataItem* item = new MeasuredDataPointDataItem(name, name.c_str(), this);
		m_childItems.push_back(item);

		auto s = new ColorMapSettingContainer();
		vtkDataArray* da = md->polyData()->GetPointData()->GetArray(name.c_str());
		double range[2];
		da->GetRange(range);
		s->valueCaption = name.c_str();
		s->legend.setTitle(name.c_str());
		s->setAutoValueRange(range[0], range[1]);
		s->legend.imageSetting.setActor(impl->m_legendActor);

		impl->m_colorMapSettings.insert({name, s});
	}

	if (md->scalarNames().size() > 0) {
		auto name = md->scalarNames().at(0);
		impl->m_setting.mappingMode = Setting::MappingMode::Value;
		setTarget(name);
	}

	impl->m_toolBarWidget->hide();
	impl->m_colorMapToolBarWidget->hide();
}

MeasuredDataPointGroupDataItem::~MeasuredDataPointGroupDataItem()
{
	delete impl;
}

void MeasuredDataPointGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	emit impl->m_setting.updated();

	auto cmNode = iRIC::getChildNode(node, "ColorMaps");
	if (! cmNode.isNull()) {
		for (int i = 0; i < cmNode.childNodes().count(); ++i) {
			QDomElement elem = cmNode.childNodes().at(i).toElement();
			std::string name = iRIC::toStr(elem.attribute("name"));

			auto cs = colorMapSetting(name);
			if (cs == nullptr) {continue;}
			cs->load(elem);
		}
	}

	updateActorSetting();
}

void MeasuredDataPointGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);

	writer.writeStartElement("ColorMaps");
	for (auto pair : impl->m_colorMapSettings) {
		writer.writeStartElement("ColorMap");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

ColorMapSettingContainerI* MeasuredDataPointGroupDataItem::activeColorMapSetting() const
{
	if (m_standardItem->checkState() == Qt::Unchecked) {return nullptr;}
	if (impl->m_setting.mappingMode == Setting::MappingMode::Arbitrary) {return nullptr;}
	auto v = iRIC::toStr(impl->m_setting.value);
	auto it = impl->m_colorMapSettings.find(v);
	if (it == impl->m_colorMapSettings.end()) {return nullptr;}

	return it->second;
}

ColorMapSettingContainerI* MeasuredDataPointGroupDataItem::activeColorMapSettingWithVisibleLegend() const
{
	auto cm = activeColorMapSetting();
	if (cm == nullptr) {return nullptr;}
	if (! cm->legendSetting()->getVisible()) {return nullptr;}

	return cm;
}

void MeasuredDataPointGroupDataItem::gatherActiveColorMapLegends(std::vector<ColorMapLegendSettingContainerI*>* legends)
{
	auto cm = activeColorMapSettingWithVisibleLegend();
	if (cm != nullptr) {
		legends->push_back(cm->legendSetting());
	}
}

void MeasuredDataPointGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void MeasuredDataPointGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.max());
}

void MeasuredDataPointGroupDataItem::update()
{
	updateActorSetting();
}

QDialog* MeasuredDataPointGroupDataItem::propertyDialog(QWidget* p)
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {
		return nullptr;
	}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Points Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void MeasuredDataPointGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void MeasuredDataPointGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Contour Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

std::string MeasuredDataPointGroupDataItem::target() const
{
	return iRIC::toStr(impl->m_setting.value);
}

void MeasuredDataPointGroupDataItem::setTarget(const std::string &target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems, true);
	impl->m_setting.value = target.c_str();

	if (target == "") {
		impl->m_setting.mappingMode = Setting::MappingMode::Arbitrary;
	} else {
		impl->m_setting.mappingMode = Setting::MappingMode::Value;
	}

	updateActorSetting();
}

bool MeasuredDataPointGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (impl->m_setting.value == "") {return false;}
	if (impl->m_setting.opacity.value() == 100) {return false;}
	return true;
}

ColorMapSettingContainerI* MeasuredDataPointGroupDataItem::colorMapSetting(const std::string& target) const
{
	auto it = impl->m_colorMapSettings.find(target);
	if (it == impl->m_colorMapSettings.end()) {return nullptr;}

	return it->second;
}

std::unordered_map<std::string, ColorMapSettingContainerI*> MeasuredDataPointGroupDataItem::colorMapSettings() const
{
	return impl->m_colorMapSettings;
}

void MeasuredDataPointGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

void MeasuredDataPointGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	auto v = isAncientChecked() && isChecked();
	impl->m_toolBarWidget->setEnabled(v);
	impl->m_colorMapToolBarWidget->setEnabled(v && impl->m_setting.mappingMode == Setting::MappingMode::Value);
}

void MeasuredDataPointGroupDataItem::updateActorSetting()
{
	impl->updateCheckState();

	impl->m_colorMapToolBarWidget->setDisabled(true);
	impl->m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	vtkPolyData* polyData = nullptr;
	if (impl->m_setting.shapeMode == Setting::ShapeMode::Points) {
		polyData = md->pointData();
	} else if (impl->m_setting.shapeMode == Setting::ShapeMode::Surface) {
		polyData = md->polyData();
	}

	if (impl->m_setting.mappingMode == Setting::MappingMode::Arbitrary) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(polyData);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();

		impl->m_actor->GetProperty()->SetColor(impl->m_setting.color);
	} else {
		auto value = iRIC::toStr(impl->m_setting.value);
		polyData->GetPointData()->SetActiveScalars(value.c_str());
		auto cs = impl->m_colorMapSettings.at(value);
		auto mapper = cs->buildPointDataMapper(polyData);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();

		impl->m_colorMapToolBarWidget->setEnabled(true);
		impl->m_colorMapToolBarWidget->setSetting(cs);
	}

	auto prop = impl->m_actor->GetProperty();
	prop->SetOpacity(impl->m_setting.opacity);
	prop->SetPointSize(impl->m_setting.pointSize);

	m_actorCollection->AddItem(impl->m_actor);

	assignActorZValues(m_zDepthRange);

	fileDataItem()->updateColorMapLegendsVisibility();
}

void MeasuredDataPointGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
}

void MeasuredDataPointGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void MeasuredDataPointGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
}

void MeasuredDataPointGroupDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	updateActorSetting();
}

void MeasuredDataPointGroupDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	fileDataItem()->updateColorMapLegendsVisibility();
}

bool MeasuredDataPointGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	impl->m_toolBarWidget->setParent(toolBar);
	impl->m_toolBarWidget->show();
	toolBar->addWidget(impl->m_toolBarWidget);
	toolBar->addSeparator();

	impl->m_colorMapToolBarWidget->setParent(toolBar);
	impl->m_colorMapToolBarWidget->show();
	toolBar->addWidget(impl->m_colorMapToolBarWidget);

	return true;
}

MeasuredDataFileDataItem* MeasuredDataPointGroupDataItem::fileDataItem() const
{
	return dynamic_cast<MeasuredDataFileDataItem*>(parent());
}
