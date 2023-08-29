#include "../post3dwindowgraphicsview.h"
#include "post3dwindownodevectorarrowdataitem.h"
#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindownodevectorarrowtopdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodevectorarrowgroupdataitem_settingeditwidget.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/boolcontainerwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <vtkActor2D.h>
#include <vtkAppendPolyData.h>
#include <vtkGeometryFilter.h>
#include <vtkSmartPointer.h>

Post3dWindowNodeVectorArrowGroupDataItem::Post3dWindowNodeVectorArrowGroupDataItem(const std::string& target, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"test", QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_target {target},
	m_legendActor {vtkActor2D::New()},
	m_lengthLegendVisibilityWidget {new BoolContainerWidget(mainWindow())}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	auto r = renderer();
	r->AddActor2D(m_legendActor);

	auto gt = data()->gridType();
	std::string nameX = target;
	nameX.append("X");
	auto captionX = gt->output(nameX)->caption();
	auto caption = captionX.left(captionX.length() - 1);
	m_standardItem->setText(caption);

	m_setting.target = target.c_str();
	m_setting.legend.imageSetting.setActor(m_legendActor);
	m_setting.legend.imageSetting.controller()->setItem(this);
	m_setting.legend.title = caption;

	for (const auto& pair : data()->data()->valueRangeSet().pointDataValueRanges()) {
		auto cs = new ColorMapSettingContainer();
		auto caption = gt->output(pair.first)->caption();
		cs->valueCaption = caption;
		cs->legend.title = caption;
		cs->setAutoValueRange(pair.second);

		auto actor = vtkActor2D::New();
		r->AddActor2D(actor);

		cs->legend.imageSetting.setActor(actor);
		cs->legend.imageSetting.controller()->setItem(this);

		m_colorMapSettings.insert({pair.first, cs});
		m_colorMapActors.push_back(actor);
	}

	m_lengthLegendVisibilityWidget->setText(tr("Legend of Length"));
	m_lengthLegendVisibilityWidget->hide();
	m_lengthLegendVisibilityWidget->setContainer(&m_setting.legend.visible);
	connect(m_lengthLegendVisibilityWidget, &BoolContainerWidget::toggled, [=](){
		pushUpdateActorSettingCommand(m_lengthLegendVisibilityWidget->createModifyCommand(), this);
	});
}

Post3dWindowNodeVectorArrowGroupDataItem::~Post3dWindowNodeVectorArrowGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();

	for (const auto actor : m_colorMapActors) {
		r->RemoveActor2D(actor);
		actor->Delete();
	}
}

void Post3dWindowNodeVectorArrowGroupDataItem::update()
{
	updateActorSetting();
}

const std::string& Post3dWindowNodeVectorArrowGroupDataItem::target() const
{
	return m_target;
}

ColorMapSettingContainerI* Post3dWindowNodeVectorArrowGroupDataItem::colorMapSetting(const std::string& name) const
{
	return m_colorMapSettings.at(name);
}

void Post3dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	QDomNodeList children = node.childNodes();
	int idx = 1;
	for (int i = 0; i < children.size(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "Arrow") {
			auto label = QString("Face%1").arg(idx);
			auto item = new Post3dWindowNodeVectorArrowDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
			++ idx;
		}
	}
	updateActorSetting();
}

void Post3dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);

	for (auto item : m_childItems) {
		writer.writeStartElement("Arrow");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

std::vector<Post3dWindowNodeVectorArrowDataItem::Setting> Post3dWindowNodeVectorArrowGroupDataItem::faceSettings()
{
	std::vector<Post3dWindowNodeVectorArrowDataItem::Setting> ret;

	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*> (item);
		ret.push_back(item2->setting());
	}

	return ret;
}

void Post3dWindowNodeVectorArrowGroupDataItem::setFaceSettings(const std::vector<Post3dWindowNodeVectorArrowDataItem::Setting>& settings)
{
	clearChildItems();
	updateItemMap();

	auto tItem = topDataItem();
	int idx = 1;
	for (const auto& s : settings) {
		auto label = tr("Face%1").arg(idx);
		auto item = new Post3dWindowNodeVectorArrowDataItem(label, this);
		item->setSetting(s);
		item->updateZScale(tItem->zScale());

		m_childItems.push_back(item);
		++ idx;
	}

	updateItemMap();

	auto oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

void Post3dWindowNodeVectorArrowGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());
	for (const auto cm : activeColorMaps()) {
		cm->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v, true);
		controllers.push_back(cm->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post3dWindowNodeVectorArrowGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());
	for (const auto cm : activeColorMaps()) {
		cm->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v, true);
		controllers.push_back(cm->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post3dWindowNodeVectorArrowGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());
	for (const auto cm : activeColorMaps()) {
		cm->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v, true);
		controllers.push_back(cm->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

bool Post3dWindowNodeVectorArrowGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_lengthLegendVisibilityWidget->setParent(toolBar);
	m_lengthLegendVisibilityWidget->show();

	toolBar->addWidget(m_lengthLegendVisibilityWidget);
	return true;
}

void Post3dWindowNodeVectorArrowGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	if (m_childItems.size() == 0) {return;}

	m_setting.legend.imageSetting.controller()->handleResize(event, v);
	for (const auto cm : activeColorMaps()) {
		cm->legendSetting()->imgSetting()->controller()->handleResize(event, v);
	}
}

void Post3dWindowNodeVectorArrowGroupDataItem::showPropertyDialog()
{
	return showPropertyDialogModeless();
}

QDialog* Post3dWindowNodeVectorArrowGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Arrows Setting"));
	dialog->resize(900, 650);

	return dialog;
}

void Post3dWindowNodeVectorArrowGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	auto v = isAncientChecked() && isChecked();
	m_lengthLegendVisibilityWidget->setEnabled(v);
}

void Post3dWindowNodeVectorArrowGroupDataItem::updateActorSetting()
{
	m_legendActor->VisibilityOff();
	for (auto actor : m_colorMapActors) {
		actor->VisibilityOff();
	}
	m_actor2DCollection->RemoveAllItems();

	if (m_childItems.size() == 0) {return;}

	// update shape and color setting with that of the first face;
	auto firstSetting = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*>(m_childItems.at(0))->setting().arrow;
	m_setting.arrowSize = firstSetting.arrowSize;
	m_setting.lineWidth = firstSetting.lineWidth;
	m_setting.colorMode = firstSetting.colorMode;
	m_setting.customColor = firstSetting.customColor;

	// update standard length
	auto mergedData = vtkSmartPointer<vtkAppendPolyData>::New();
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*> (child);
		auto grid = item->faceGrid();
		if (grid == nullptr) {continue;}

		auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
		filter->SetInputData(grid);
		filter->Update();
		grid->Delete();
		mergedData->AddInputData(filter->GetOutput());
	}
	mergedData->Update();

	m_setting.updateStandardValueIfNeeded(mergedData->GetOutput()->GetPointData());

	auto v = dataModel()->graphicsView();
	m_setting.legend.imageSetting.apply(v);
	for (const auto cm : activeColorMaps()) {
		cm->legendSetting()->imgSetting()->apply(v);
	}

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*> (child);
		item->update();
	}
}

Post3dWindowNodeVectorArrowTopDataItem* Post3dWindowNodeVectorArrowGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post3dWindowNodeVectorArrowTopDataItem*> (parent());
}

const ArrowsSettingContainer& Post3dWindowNodeVectorArrowGroupDataItem::setting() const
{
	return m_setting;
}

PostZoneDataContainer* Post3dWindowNodeVectorArrowGroupDataItem::data() const
{
	auto zItem = dynamic_cast<Post3dWindowZoneDataItem*> (parent()->parent());
	return zItem->dataContainer();
}

std::unordered_set<ColorMapSettingContainerI*> Post3dWindowNodeVectorArrowGroupDataItem::activeColorMaps() const
{
	std::unordered_set<ColorMapSettingContainerI*> ret;

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*> (child);
		if (! item->isAncientChecked() || ! item->isChecked()) {continue;}
		if (item->setting().arrow.colorMode != ArrowsSettingContainer::ColorMode::ByScalar) {continue;}

		ret.insert(m_colorMapSettings.at(iRIC::toStr(item->setting().arrow.colorTarget)));
	}

	return ret;
}
