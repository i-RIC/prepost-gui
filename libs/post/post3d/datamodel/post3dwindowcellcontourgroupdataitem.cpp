#include "../post3dwindowgraphicsview.h"
#include "post3dwindowcellcontourdataitem.h"
#include "post3dwindowcellcontourgroupdataitem.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowcellcontourgroupdataitem_propertydialog.h"

#include <guibase/objectbrowserview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>

#include <vtkActor2D.h>

Post3dWindowCellContourGroupDataItem::Post3dWindowCellContourGroupDataItem(const std::string& target, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_target {target},
	m_colorMapSetting {},
	m_legendActor {vtkActor2D::New()}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	renderer()->AddActor2D(m_legendActor);

	m_colorMapSetting.legend.imageSetting.setActor(m_legendActor);
	m_colorMapSetting.legend.imageSetting.controller()->setItem(this);
	m_colorMapSetting.legend.title = data()->gridType()->output(target)->caption();
	m_colorMapSetting.legend.visibilityMode = ColorMapLegendSettingContainer::VisibilityMode::Always;
	m_colorMapSetting.setAutoValueRange(valueRange());

	auto gType = zoneDataItem()->gridTypeDataItem()->gridType();
	m_standardItem->setText(gType->solutionCaption(m_target));

	informSelection(dataModel()->graphicsView());
}

Post3dWindowCellContourGroupDataItem::~Post3dWindowCellContourGroupDataItem()
{
	renderer()->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();
}

void Post3dWindowCellContourGroupDataItem::update()
{
	updateActorSettings();
}

const std::string& Post3dWindowCellContourGroupDataItem::target() const
{
	return m_target;
}

void Post3dWindowCellContourGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post3dWindowCellContourGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_colorMapSetting.load(node);

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "RangeSetting") {
			QString label = childElem.attribute("label");
			auto item = new Post3dWindowCellContourDataItem(label, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}

	updateActorSettings();
}

void Post3dWindowCellContourGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_colorMapSetting.save(writer);

	for (int i = 0; i < m_childItems.size(); ++i) {
		auto fitem = dynamic_cast<Post3dWindowCellContourDataItem*>(m_childItems.at(i));
		writer.writeStartElement("RangeSetting");
		fitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post3dWindowCellContourGroupDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

void Post3dWindowCellContourGroupDataItem::updateActorSettings()
{
	m_colorMapSetting.setAutoValueRange(valueRange());
	m_colorMapSetting.legend.imageSetting.apply(dataModel()->graphicsView());

	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowCellContourDataItem*> (child);
		item->update();
	}
	updateVisibilityWithoutRendering();
}

const ValueRangeContainer& Post3dWindowCellContourGroupDataItem::valueRange() const
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	return gtItem->cellValueRange(m_target);
}

Post3dWindowZoneDataItem* Post3dWindowCellContourGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent());
}

PostZoneDataContainer* Post3dWindowCellContourGroupDataItem::data() const
{
	return zoneDataItem()->dataContainer();
}

std::vector<Post3dWindowCellRangeSettingContainer> Post3dWindowCellContourGroupDataItem::ranges() const
{
	std::vector<Post3dWindowCellRangeSettingContainer> ret;

	for (auto item : m_childItems) {
		auto rangeItem = dynamic_cast<Post3dWindowCellContourDataItem*> (item);
		ret.push_back(rangeItem->setting());
	}

	return ret;
}

void Post3dWindowCellContourGroupDataItem::setRanges(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges)
{
	clearChildItems();
	updateItemMap();

	auto tItem = dynamic_cast<Post3dWindowCellContourGroupTopDataItem*> (parent());
	int idx = 1;
	for (const auto& range : ranges) {
		auto label = tr("Range%1").arg(idx);
		auto item = new Post3dWindowCellContourDataItem(label, this);
		item->setSetting(range);
		item->updateZScale(tItem->zScale());

		m_childItems.push_back(item);
		++ idx;
	}

	updateItemMap();

	auto oview = dataModel()->objectBrowserView();
	oview->expand(standardItem()->index());
}

void Post3dWindowCellContourGroupDataItem::informSelection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleSelection(v);
}

void Post3dWindowCellContourGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleDeselection(v);
}

void Post3dWindowCellContourGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void Post3dWindowCellContourGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post3dWindowCellContourGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

void Post3dWindowCellContourGroupDataItem::doHandleResize(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleResize(v);
}
