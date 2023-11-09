#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridshapedataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowgridshapedataitem_settingeditwidget.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4gridutil.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
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

	updateActorSetting();
}

void Post3dWindowGridShapeDataItem::update()
{
	updateActorSetting();
}

void Post3dWindowGridShapeDataItem::updateActorSetting()
{
	m_setting.outlineActor()->VisibilityOff();
	m_setting.wireframeActor()->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {return;}

	auto grid = dynamic_cast<v4Structured3dGrid*> (cont->gridData()->grid());
	auto data = grid->vtkConcreteData()->concreteData();
	m_setting.update(actorCollection(), actor2DCollection(), data, data, grid->vtkIndexData(), v4GridUtil::LABEL_NAME);

	updateVisibilityWithoutRendering();
}

Post3dWindowZoneDataItem* Post3dWindowGridShapeDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}

void Post3dWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSetting();
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
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {return nullptr;}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Grid Shape Display Setting"));

	return dialog;
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
