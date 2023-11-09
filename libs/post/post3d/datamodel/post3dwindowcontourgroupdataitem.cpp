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
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <vtkActor2D.h>

Post3dWindowContourGroupDataItem::Post3dWindowContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	impl->m_target = target;

	auto gType = data()->gridType();
	auto caption = gType->outputCaption(target);

	SolverDefinitionGridOutput* output = nullptr;
	QRegExp re("^(.+) \\(magnitude\\)$");
	auto pos = re.indexIn(target.c_str());
	if (pos >= 0) {
		auto target2 = iRIC::toStr(re.cap(1)) + "X";
		output = gType->output(target2);
	} else {
		output = gType->output(target);
	}

	m_standardItem->setText(caption);
	auto cs = output->createColorMapSettingContainer();
	cs->legendSetting()->setTitle(caption);
	impl->m_setting.colorMapSetting = cs;
	impl->m_setting.contourSetting.setColorMapSetting(cs);
	cs->legendSetting()->imgSetting()->setActor(impl->m_legendActor);
	cs->legendSetting()->imgSetting()->controller()->setItem(this);
	cs->setAutoValueRange(valueRange());

	impl->m_colorMapToolBarWidget->hide();
	impl->m_colorMapToolBarWidget->setSetting(cs);
	connect(impl->m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(impl->m_colorMapToolBarWidget->modifiedSetting(), cs);
		pushUpdateActorSettingCommand(com, this);
	});

	impl->m_opacityToolBarWidget->hide();
	impl->m_opacityToolBarWidget->setContainer(&impl->m_setting.opacity);
	connect(impl->m_opacityToolBarWidget, &OpacityContainerWidget::updated, [=](){
		auto com = impl->m_opacityToolBarWidget->createModifyCommand();
		pushUpdateActorSettingCommand(com, this, false);
	});
	renderer()->AddActor2D(impl->m_legendActor);

	informSelection(dataModel()->graphicsView());
}

Post3dWindowContourGroupDataItem::~Post3dWindowContourGroupDataItem()
{
	renderer()->RemoveActor2D(impl->m_legendActor);
}

void Post3dWindowContourGroupDataItem::update()
{
	updateActorSetting();
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
	impl->m_setting.colorMapSetting->setAutoValueRange(valueRange());
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->apply(dataModel()->graphicsView());

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowContourDataItem*> (child);
		item->update();
	}
	updateVisibilityWithoutRendering();
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

v4PostZoneDataContainer* Post3dWindowContourGroupDataItem::data() const
{
	return zoneDataItem()->v4DataContainer();
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

		m_childItems.push_back(item);
		++ idx;
	}

	updateItemMap();

	auto oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

void Post3dWindowContourGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
}

void Post3dWindowContourGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void Post3dWindowContourGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
}

void Post3dWindowContourGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleResize(event, v);
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
