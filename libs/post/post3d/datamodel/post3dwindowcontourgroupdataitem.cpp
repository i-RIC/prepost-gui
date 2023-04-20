#include "../post3dwindowgraphicsview.h"
#include "post3dwindowcontourdataitem.h"
#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowcontourgroupdataitem_propertydialog.h"

#include <guibase/objectbrowserview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>

#include <vtkActor2D.h>

Post3dWindowContourGroupDataItem::Post3dWindowContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_target {target},
	m_colorMapSetting {},
	m_legendActor {vtkActor2D::New()},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(mainWindow())}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor2D(m_legendActor);
	m_colorMapSetting.legend.imageSetting.setActor(m_legendActor);
	m_colorMapSetting.legend.imageSetting.controller()->setItem(this);
	m_colorMapSetting.legend.title = data()->gridType()->output(target)->caption();
	m_colorMapSetting.setAutoValueRange(valueRange());

	auto gType = zoneDataItem()->gridTypeDataItem()->gridType();
	m_standardItem->setText(gType->solutionCaption(m_target));

	m_colorMapToolBarWidget->hide();
	m_colorMapToolBarWidget->setSetting(&m_colorMapSetting);
	connect(m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(m_colorMapToolBarWidget->modifiedSetting(), &m_colorMapSetting);
		pushUpdateActorSettingCommand(com, this);
	});

	informSelection(dataModel()->graphicsView());
}

Post3dWindowContourGroupDataItem::~Post3dWindowContourGroupDataItem()
{
	renderer()->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();
}

void Post3dWindowContourGroupDataItem::update()
{
	updateActorSetting();
}

const std::string& Post3dWindowContourGroupDataItem::target() const
{
	return m_target;
}

void Post3dWindowContourGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post3dWindowContourGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_colorMapSetting.load(node);

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
	m_colorMapSetting.save(writer);

	for (int i = 0; i < m_childItems.size(); ++i) {
		auto fitem = dynamic_cast<Post3dWindowContourDataItem*>(m_childItems.at(i));
		writer.writeStartElement("FaceSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post3dWindowContourGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new PropertyDialog(this, p);
	dialog->setWindowTitle(tr("Contour Setting (%1)").arg(standardItem()->text()));
	return dialog;
}

void Post3dWindowContourGroupDataItem::updateActorSetting()
{
	m_colorMapSetting.setAutoValueRange(valueRange());
	m_colorMapSetting.legend.imageSetting.apply(dataModel()->graphicsView());

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowContourDataItem*> (child);
		item->update();
	}
	updateVisibilityWithoutRendering();
}

const ValueRangeContainer& Post3dWindowContourGroupDataItem::valueRange() const
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	return gtItem->nodeValueRange(m_target);
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

		m_childItems.push_back(item);
		++ idx;
	}

	updateItemMap();

	auto oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

void Post3dWindowContourGroupDataItem::informSelection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleSelection(v);
}

void Post3dWindowContourGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleDeselection(v);
}

void Post3dWindowContourGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void Post3dWindowContourGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post3dWindowContourGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

void Post3dWindowContourGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleResize(event, v);
}

bool Post3dWindowContourGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_colorMapToolBarWidget->setParent(toolBar);
	m_colorMapToolBarWidget->show();

	toolBar->addWidget(m_colorMapToolBarWidget);
	return true;
}
