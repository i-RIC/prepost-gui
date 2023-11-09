#include "../gridbirdeyewindowgraphicsview.h"
#include "gridbirdeyewindowgridshapedataitem.h"
#include "gridbirdeyewindowzonedataitem.h"
#include "private/gridbirdeyewindowgridshapedataitem_impl.h"
#include "private/gridbirdeyewindowgridshapedataitem_settingeditwidget.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkWarpScalar.h>

namespace {
	std::string ELEVATION = "Elevation";
}

GridBirdEyeWindowGridShapeDataItem::GridBirdEyeWindowGridShapeDataItem(GridBirdEyeWindowDataItem* parent) :
	GridBirdEyeWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_gridWarp {vtkWarpScalar::New()},
	m_labelWarp {vtkWarpScalar::New()},
	impl {new Impl {}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto grid = zoneDataItem()->grid();
	if (grid != nullptr) {
		if (dynamic_cast<v4Structured2dGrid*> (grid->grid()) == nullptr) {
			impl->m_setting.gridShape.shape = GridShapeSettingContainer::Shape::Wireframe;
		}
	}

	impl->m_setting.elevationTarget = ELEVATION.c_str();

	setupActors();
}

GridBirdEyeWindowGridShapeDataItem::~GridBirdEyeWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_setting.gridShape.outlineActor());
	r->RemoveActor(impl->m_setting.gridShape.wireframeActor());
	r->RemoveActor2D(impl->m_setting.gridShape.indexActor());

	m_gridWarp->Delete();
	m_labelWarp->Delete();

	delete impl;
}

void GridBirdEyeWindowGridShapeDataItem::setupActors()
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

void GridBirdEyeWindowGridShapeDataItem::update()
{
	updateActorSetting();
}

void GridBirdEyeWindowGridShapeDataItem::updateActorSetting()
{
	auto g = zoneDataItem()->grid();
	if (g == nullptr) {return;}

	m_gridWarp->SetInputData(g->grid()->vtkData()->data());
	m_gridWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(impl->m_setting.elevationTarget).c_str());
	m_gridWarp->Update();

	auto dummyLabelData = vtkSmartPointer<vtkPolyData>::New();

	impl->m_setting.gridShape.update(actorCollection(), actor2DCollection(),
									 m_gridWarp->GetOutput() , m_gridWarp->GetOutput(),
									 dummyLabelData,
									 iRIC::toStr(PostZoneDataContainer::labelName));

	updateVisibilityWithoutRendering();
}

void GridBirdEyeWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	updateActorSetting();
}

void GridBirdEyeWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void GridBirdEyeWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

void GridBirdEyeWindowGridShapeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GridBirdEyeWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Grid Shape Setting"));

	return dialog;
}

void GridBirdEyeWindowGridShapeDataItem::informSelection(VTKGraphicsView* v)
{
	impl->m_setting.gridShape.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::selectedOutlineWidth * v->devicePixelRatioF());
	updateVisibility();
}

void GridBirdEyeWindowGridShapeDataItem::informDeselection(VTKGraphicsView* v)
{
	impl->m_setting.gridShape.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());
}

void GridBirdEyeWindowGridShapeDataItem::innerUpdateZScale(double scale)
{
	impl->m_setting.gridShape.setScale(1, 1, scale);
}

GridBirdEyeWindowZoneDataItem* GridBirdEyeWindowGridShapeDataItem::zoneDataItem() const
{
	return dynamic_cast<GridBirdEyeWindowZoneDataItem*> (parent());
}
