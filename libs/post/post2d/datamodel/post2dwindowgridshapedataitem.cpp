#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowgriddataitemi.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridshapedataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/grid/v4gridutil.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
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

	updateActorSetting();
}

void Post2dWindowGridShapeDataItem::update()
{
	updateActorSetting();
}

void Post2dWindowGridShapeDataItem::updateActorSetting()
{
	auto grid2d = gridDataItem()->grid();
	m_setting.update(actorCollection(), actor2DCollection(), grid2d->vtkData()->data(), grid2d->vtkFilteredData(), grid2d->vtkFilteredIndexData(), v4GridUtil::LABEL_NAME);

	updateVisibilityWithoutRendering();
}

Post2dWindowGridDataItemI* Post2dWindowGridShapeDataItem::gridDataItem() const
{
	return dynamic_cast<Post2dWindowGridDataItemI*> (parent());
}

void Post2dWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSetting();
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
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Grid Shape Display Setting"));

	return dialog;
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
