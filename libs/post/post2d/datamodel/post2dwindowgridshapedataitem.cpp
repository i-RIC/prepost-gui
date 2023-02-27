#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridshapedataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

Post2dWindowGridShapeDataItem::Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();
}

Post2dWindowGridShapeDataItem::~Post2dWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_setting.outlineActor());
	r->RemoveActor(m_setting.wireframeActor());
	r->RemoveActor2D(m_setting.indexActor());
}

void Post2dWindowGridShapeDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_setting.outlineActor());
	r->AddActor(m_setting.wireframeActor());
	r->AddActor2D(m_setting.indexActor());

	auto v = dataModel()->graphicsView();
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());

	updateActorSettings();
}

void Post2dWindowGridShapeDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowGridShapeDataItem::updateActorSettings()
{
	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}

	auto data = cont->data()->data();
	m_setting.update(actorCollection(), actor2DCollection(), data, data, cont->labelData(), iRIC::toStr(PostZoneDataContainer::labelName));

	updateVisibilityWithoutRendering();
}

Post2dWindowZoneDataItem* Post2dWindowGridShapeDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

void Post2dWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSettings();
}

void Post2dWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

void Post2dWindowGridShapeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

void Post2dWindowGridShapeDataItem::informSelection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::selectedOutlineWidth * v->devicePixelRatioF());
	updateVisibility();
}

void Post2dWindowGridShapeDataItem::informDeselection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());
}

void Post2dWindowGridShapeDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void Post2dWindowGridShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_setting.outlineActor()->SetPosition(0, 0, range.max());
	m_setting.wireframeActor()->SetPosition(0, 0, range.min());
}
