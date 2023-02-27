#include "../post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include "private/post2dbirdeyewindowgridshapedataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkWarpScalar.h>

Post2dBirdEyeWindowGridShapeDataItem::Post2dBirdEyeWindowGridShapeDataItem(Post2dBirdEyeWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_gridWarp {vtkWarpScalar::New()},
	m_labelWarp {vtkWarpScalar::New()},
	m_setting {},
	m_elevationTarget {"elevationTarget"}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();
	if (vtkStructuredGrid::SafeDownCast(cont->data()->data()) == nullptr) {
		m_setting.shape = GridShapeSettingContainer::Shape::Wireframe;
	}

	m_elevationTarget = cont->elevationName();

	setupActors();
}

Post2dBirdEyeWindowGridShapeDataItem::~Post2dBirdEyeWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_setting.outlineActor());
	r->RemoveActor(m_setting.wireframeActor());
	r->RemoveActor2D(m_setting.indexActor());

	m_gridWarp->Delete();
	m_labelWarp->Delete();
}

void Post2dBirdEyeWindowGridShapeDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_setting.outlineActor());
	r->AddActor(m_setting.wireframeActor());
	r->AddActor2D(m_setting.indexActor());

	auto v = dataModel()->graphicsView();
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());

	m_gridWarp->UseNormalOn();
	m_gridWarp->SetNormal(0, 0, 1);
	m_gridWarp->SetScaleFactor(1);

	m_labelWarp->UseNormalOn();
	m_labelWarp->SetNormal(0, 0, 1);
	m_labelWarp->SetScaleFactor(1);

	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::update()
{
	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::updateActorSettings()
{

	auto cont = zoneDataItem()->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}

	m_gridWarp->SetInputData(cont->data()->data());
	m_gridWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_elevationTarget).c_str());
	m_gridWarp->Update();

	m_labelWarp->SetInputData(cont->labelData());
	m_labelWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(m_elevationTarget).c_str());
	m_labelWarp->Update();

	m_setting.update(actorCollection(), actor2DCollection(),
									 m_gridWarp->GetOutput() , m_gridWarp->GetOutput(),
									 m_labelWarp->GetOutput(),
									 iRIC::toStr(PostZoneDataContainer::labelName));

	updateVisibilityWithoutRendering();
}

void Post2dBirdEyeWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_elevationTarget.load(node);
	m_setting.load(node);

	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_elevationTarget.save(writer);
	m_setting.save(writer);
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
	return new PropertyDialog(this, p);
}

void Post2dBirdEyeWindowGridShapeDataItem::informSelection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::selectedOutlineWidth * v->devicePixelRatioF());
	updateVisibility();
}

void Post2dBirdEyeWindowGridShapeDataItem::informDeselection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());
}

void Post2dBirdEyeWindowGridShapeDataItem::innerUpdateZScale(double scale)
{
	m_setting.setScale(1, 1, scale);
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowGridShapeDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowZoneDataItem*> (parent());
}
