#include "../post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include "private/post2dbirdeyewindowgridshapedataitem_impl.h"
#include "private/post2dbirdeyewindowgridshapedataitem_settingeditwidget.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkWarpScalar.h>

Post2dBirdEyeWindowGridShapeDataItem::Post2dBirdEyeWindowGridShapeDataItem(Post2dBirdEyeWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_gridWarp {vtkWarpScalar::New()},
	m_labelWarp {vtkWarpScalar::New()},
	impl {new Impl {}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();
	if (vtkStructuredGrid::SafeDownCast(cont->data()->data()) == nullptr) {
		impl->m_setting.gridShape.shape = GridShapeSettingContainer::Shape::Wireframe;
	}

	impl->m_setting.elevationTarget = cont->elevationName();

	setupActors();
}

Post2dBirdEyeWindowGridShapeDataItem::~Post2dBirdEyeWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_setting.gridShape.outlineActor());
	r->RemoveActor(impl->m_setting.gridShape.wireframeActor());
	r->RemoveActor2D(impl->m_setting.gridShape.indexActor());

	m_gridWarp->Delete();
	m_labelWarp->Delete();

	delete impl;
}

void Post2dBirdEyeWindowGridShapeDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_setting.gridShape.outlineActor());
	r->AddActor(impl->m_setting.gridShape.wireframeActor());
	r->AddActor2D(impl->m_setting.gridShape.indexActor());

	auto v = dataModel()->graphicsView();
	impl->m_setting.gridShape.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());

	m_gridWarp->UseNormalOn();
	m_gridWarp->SetNormal(0, 0, 1);
	m_gridWarp->SetScaleFactor(1);

	m_labelWarp->UseNormalOn();
	m_labelWarp->SetNormal(0, 0, 1);
	m_labelWarp->SetScaleFactor(1);

	updateActorSetting();
}

void Post2dBirdEyeWindowGridShapeDataItem::update()
{
	updateActorSetting();
}

void Post2dBirdEyeWindowGridShapeDataItem::updateActorSetting()
{

	auto cont = zoneDataItem()->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}

	m_gridWarp->SetInputData(cont->data()->data());
	m_gridWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(impl->m_setting.elevationTarget).c_str());
	m_gridWarp->Update();

	m_labelWarp->SetInputData(cont->labelData());
	m_labelWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(impl->m_setting.elevationTarget).c_str());
	m_labelWarp->Update();

	impl->m_setting.gridShape.update(actorCollection(), actor2DCollection(),
									 m_gridWarp->GetOutput() , m_gridWarp->GetOutput(),
									 m_labelWarp->GetOutput(),
									 iRIC::toStr(PostZoneDataContainer::labelName));

	updateVisibilityWithoutRendering();
}

void Post2dBirdEyeWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	updateActorSetting();
}

void Post2dBirdEyeWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void Post2dBirdEyeWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

void Post2dBirdEyeWindowGridShapeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dBirdEyeWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Grid Shape Setting"));

	return dialog;
}

void Post2dBirdEyeWindowGridShapeDataItem::informSelection(VTKGraphicsView* v)
{
	impl->m_setting.gridShape.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::selectedOutlineWidth * v->devicePixelRatioF());
	updateVisibility();
}

void Post2dBirdEyeWindowGridShapeDataItem::informDeselection(VTKGraphicsView* v)
{
	impl->m_setting.gridShape.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());
}

void Post2dBirdEyeWindowGridShapeDataItem::innerUpdateZScale(double scale)
{
	impl->m_setting.gridShape.setScale(1, 1, scale);
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowGridShapeDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowZoneDataItem*> (parent());
}
