#include "../post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include "private/post2dbirdeyewindowgridshapedataitem_impl.h"
#include "private/post2dbirdeyewindowgridshapedataitem_settingeditwidget.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4gridutil.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/stringtool.h>

#include <vtkWarpScalar.h>

Post2dBirdEyeWindowGridShapeDataItem::Post2dBirdEyeWindowGridShapeDataItem(Post2dBirdEyeWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	impl {new Impl {}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (cont->gridData()->grid());
	if (sGrid == nullptr) {
		impl->m_setting.gridShape.shape = GridShapeSettingContainer::Shape::Wireframe;
	}

	impl->m_setting.elevationTarget = cont->gridData()->elevationName(v4SolutionGrid::Position::Node).c_str();

	setupActors();
}

Post2dBirdEyeWindowGridShapeDataItem::~Post2dBirdEyeWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_setting.gridShape.outlineActor());
	r->RemoveActor(impl->m_setting.gridShape.wireframeActor());
	r->RemoveActor2D(impl->m_setting.gridShape.indexActor());
}

void Post2dBirdEyeWindowGridShapeDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_setting.gridShape.outlineActor());
	r->AddActor(impl->m_setting.gridShape.wireframeActor());
	r->AddActor2D(impl->m_setting.gridShape.indexActor());

	auto v = dataModel()->graphicsView();
	impl->m_setting.gridShape.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());

	impl->m_gridWarp->UseNormalOn();
	impl->m_gridWarp->SetNormal(0, 0, 1);
	impl->m_gridWarp->SetScaleFactor(1);

	impl->m_labelWarp->UseNormalOn();
	impl->m_labelWarp->SetNormal(0, 0, 1);
	impl->m_labelWarp->SetScaleFactor(1);

	updateActorSetting();
}

void Post2dBirdEyeWindowGridShapeDataItem::update()
{
	updateActorSetting();
}

void Post2dBirdEyeWindowGridShapeDataItem::updateActorSetting()
{
	impl->m_setting.gridShape.outlineActor()->VisibilityOff();
	impl->m_setting.gridShape.wireframeActor()->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {return;}

	impl->m_gridWarp->SetInputData(cont->gridData()->grid()->vtkData()->data());
	impl->m_gridWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(impl->m_setting.elevationTarget).c_str());
	impl->m_gridWarp->Update();

	auto grid2d = dynamic_cast<v4Grid2d*> (cont->gridData()->grid());
	impl->m_labelWarp->SetInputData(grid2d->vtkFilteredIndexData());
	impl->m_labelWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(impl->m_setting.elevationTarget).c_str());
	impl->m_labelWarp->Update();

	impl->m_setting.gridShape.update(actorCollection(), actor2DCollection(),
									 impl->m_gridWarp->GetOutput(), impl->m_gridWarp->GetOutput(),
									 impl->m_labelWarp->GetOutput(),
									 v4GridUtil::LABEL_NAME);

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
	if (zoneDataItem()->v4DataContainer() == nullptr) {return nullptr;}

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
