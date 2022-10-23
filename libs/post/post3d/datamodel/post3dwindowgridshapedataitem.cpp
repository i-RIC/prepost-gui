#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridshapedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowgridshapedataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

Post3dWindowGridShapeDataItem::Post3dWindowGridShapeDataItem(Post3dWindowDataItem* parent) :
	Post3dWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();
}

Post3dWindowGridShapeDataItem::~Post3dWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_setting.outlineActor());
	r->RemoveActor(m_setting.wireframeActor());
	r->RemoveActor2D(m_setting.indexActor());
}

void Post3dWindowGridShapeDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_setting.outlineActor());
	r->AddActor(m_setting.wireframeActor());
	r->AddActor2D(m_setting.indexActor());

	auto v = dataModel()->graphicsView();
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());

	updateActorSettings();
}

void Post3dWindowGridShapeDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowGridShapeDataItem::updateActorSettings()
{
	auto cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}

	auto data = cont->data()->data();
	m_setting.update(actorCollection(), actor2DCollection(), data, data, cont->labelData(), iRIC::toStr(PostZoneDataContainer::labelName));

	updateVisibilityWithoutRendering();
}

Post3dWindowZoneDataItem* Post3dWindowGridShapeDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}

void Post3dWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSettings();
}

void Post3dWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post3dWindowGridShapeDataItem::innerUpdateZScale(double scale)
{
	m_setting.setScale(1, 1, scale);
}

void Post3dWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

void Post3dWindowGridShapeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post3dWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

void Post3dWindowGridShapeDataItem::informSelection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::selectedOutlineWidth * v->devicePixelRatioF());
	updateVisibility();
}

void Post3dWindowGridShapeDataItem::informDeselection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());
}
