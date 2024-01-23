#include "../post3dwindowgraphicsview.h"
#include "post3dwindowcontourdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowcontourgroupdataitem_impl.h"
#include "private/post3dwindowcontourgroupdataitem_settingeditwidget.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/valuechangert.h>

#include <vtkActor2D.h>

Post3dWindowContourGroupDataItem::Post3dWindowContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl {this}}
{
	auto caption = data()->gridType()->output(target)->caption();
	setupStandardItem(Checked, NotReorderable, Deletable);

	impl->m_target = target;

	renderer()->AddActor2D(impl->m_legendActor);
	impl->m_colorMapSetting.legend.imageSetting.setActor(impl->m_legendActor);
	impl->m_colorMapSetting.legend.title = caption;
	impl->m_colorMapSetting.setAutoValueRange(valueRange());

	m_standardItem->setText(caption);

	impl->m_colorMapToolBarWidget->hide();
	impl->m_colorMapToolBarWidget->setSetting(&impl->m_colorMapSetting);
	connect(impl->m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(impl->m_colorMapToolBarWidget->modifiedSetting(), &impl->m_colorMapSetting);
		pushUpdateActorSettingCommand(com, this);
	});

	impl->m_opacityToolBarWidget->hide();
	impl->m_opacityToolBarWidget->setContainer(&impl->m_setting.opacity);
	connect(impl->m_opacityToolBarWidget, &OpacityContainerWidget::updated, [=](){
		auto com = impl->m_opacityToolBarWidget->createModifyCommand();
		pushUpdateActorSettingCommand(com, this, false);
	});

	informSelection(dataModel()->graphicsView());
}

Post3dWindowContourGroupDataItem::~Post3dWindowContourGroupDataItem()
{
	renderer()->RemoveActor2D(impl->m_legendActor);

	delete impl;
}

void Post3dWindowContourGroupDataItem::update()
{
	updateActorSetting();
}

void Post3dWindowContourGroupDataItem::updateColorMapVisibility()
{
	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);

	auto actor = impl->m_colorMapSetting.legend.imageSetting.actor();
	m_actor2DCollection->RemoveItem(actor);
	actor->VisibilityOff();

	bool visible = false;
	if (standardItem()->checkState() == Qt::Checked) {
		for (auto child : m_childItems) {
			visible = visible || child->standardItem()->checkState() == Qt::Checked;
		}
	}
	if (visible) {
		auto v = dataModel()->graphicsView();
		impl->m_colorMapSetting.legend.imageSetting.apply(v);
		m_actor2DCollection->AddItem(actor);
		updateVisibilityWithoutRendering();
	}
}


const std::string& Post3dWindowContourGroupDataItem::target() const
{
	return impl->m_target;
}

void Post3dWindowContourGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post3dWindowContourGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	impl->m_colorMapSetting.load(node);

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "FaceSetting") {
			QString label = childElem.attribute("label");
			auto item = new Post3dWindowContourDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}

	updateActorSetting();
}

void Post3dWindowContourGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
	impl->m_colorMapSetting.save(writer);

	for (int i = 0; i < m_childItems.size(); ++i) {
		auto fitem = dynamic_cast<Post3dWindowContourDataItem*>(m_childItems.at(i));
		writer.writeStartElement("FaceSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post3dWindowContourGroupDataItem::propertyDialog(QWidget* p)
{
	if (data() == nullptr) {return nullptr;}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Contour Setting (%1)").arg(standardItem()->text()));
	dialog->resize(900, 650);
	return dialog;
}

void Post3dWindowContourGroupDataItem::updateActorSetting()
{
	impl->m_colorMapSetting.setAutoValueRange(valueRange());
	impl->m_colorMapSetting.legend.imageSetting.apply(dataModel()->graphicsView());

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowContourDataItem*> (child);
		item->update();
	}
	updateColorMapVisibility();
	updateVisibilityWithoutRendering();
}

void Post3dWindowContourGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);
	updateColorMapVisibility();
}

const ValueRangeContainer& Post3dWindowContourGroupDataItem::valueRange() const
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	return gtItem->nodeValueRange(impl->m_target);
}

Post3dWindowZoneDataItem* Post3dWindowContourGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent());
}

PostZoneDataContainer* Post3dWindowContourGroupDataItem::data() const
{
	return zoneDataItem()->dataContainer();
}

std::vector<Post3dWindowFaceSettingContainer> Post3dWindowContourGroupDataItem::faces() const
{
	std::vector<Post3dWindowFaceSettingContainer> ret;

	for (auto item : m_childItems) {
		auto faceItem = dynamic_cast<Post3dWindowContourDataItem*> (item);
		ret.push_back(faceItem->setting());
	}

	return ret;
}

void Post3dWindowContourGroupDataItem::setFaces(const std::vector<Post3dWindowFaceSettingContainer>& faces)
{
	clearChildItems();
	updateItemMap();

	auto tItem = dynamic_cast<Post3dWindowContourGroupTopDataItem*> (parent());
	int idx = 1;
	for (const auto& face : faces) {
		auto label = tr("Face%1").arg(idx);
		auto item = new Post3dWindowContourDataItem(label, this);
		item->setSetting(face);
		item->updateZScale(tItem->zScale());

		Qt::CheckState cs = Qt::Unchecked;
		if (face.enabled) {
			cs = Qt::Checked;
		}
		item->standardItem()->setCheckState(cs);

		m_childItems.push_back(item);
		++ idx;
	}

	updateItemMap();

	auto oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

void Post3dWindowContourGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(this, event, v);
}

void Post3dWindowContourGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(this, event, v);
}

void Post3dWindowContourGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(this, event, v);
}

void Post3dWindowContourGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	impl->m_colorMapSetting.legend.imageSetting.controller()->handleResize(event, v);
}

bool Post3dWindowContourGroupDataItem::addToolBarButtons(QToolBar* toolBar)
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
