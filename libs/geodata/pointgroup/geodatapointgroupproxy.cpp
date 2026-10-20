#include "geodatapointgroup.h"
#include "geodatapointgrouppoint.h"
#include "geodatapointgroupproxy.h"
#include "geodatapointgroupcolorsettingdialog.h"

#include "private/geodatapointgroup_impl.h"
#include "private/geodatapointgroupproxy_displaysettingwidget.h"
#include "private/geodatapointgroupproxy_impl.h"

#include <geodata/point/geodatapoint.h>
#include <geodata/point/private/geodatapoint_impl.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/pre/geodata/private/geodataproxy_propertydialog.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <misc/zdepthrange.h>

#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageMapper.h>
#include <vtkQImageToImageSource.h>
#include <vtkProperty2D.h>

void GeoDataPointGroupProxy::Impl::setupPointsActor(vtkActor* actor, vtkPolyData* data, const GeoDataPointGroup::DisplaySetting& ds, ColorMapSettingContainerI* cm)
{
	// color
	QColor c = ds.color;

	actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());

	// mapping
	if (ds.mapping == GeoDataPointGroup::DisplaySetting::Mapping::Value && (cm != nullptr)) {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(data, true);
		actor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(data);
		actor->SetMapper(mapper);
		mapper->Delete();
	}

	// opacity
	actor->GetProperty()->SetOpacity(ds.opacity);

	// pointSize
	actor->GetProperty()->SetPointSize(ds.pointSize);
}

GeoDataPointGroupProxy::GeoDataPointGroupProxy(GeoDataPointGroup* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{
	impl->m_displaySetting.displaySetting = geodata->impl->m_displaySetting;
}

GeoDataPointGroupProxy::~GeoDataPointGroupProxy()
{
	auto r = renderer();

	for (auto actor : impl->m_imageActors) {
		r->RemoveActor2D(actor);
	}
	r->RemoveActor(impl->m_pointsActor);
	r->RemoveActor(impl->m_editTargetPointActor);

	delete impl;
}

void GeoDataPointGroupProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_pointsActor);
	col->AddItem(impl->m_pointsActor);

	// m_editTargetPointActor is added to the actor collection in updateActorSetting(), only when edit target data exists
	r->AddActor(impl->m_editTargetPointActor);
	impl->m_editTargetPointActor->VisibilityOff();

	updateActorSetting();
}

void GeoDataPointGroupProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPointGroupProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPointGroupProxy::propertyDialog(QWidget* parent)
{
	auto dialog = new PropertyDialog(this, parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Points Display Setting"));
	return dialog;
}

void GeoDataPointGroupProxy::viewOperationEndedGlobal(VTKGraphicsView* /*v*/)
{
	updateActorSetting();
}

void GeoDataPointGroupProxy::updateActorSetting()
{
	auto points = dynamic_cast<GeoDataPointGroup*> (geoData());
	auto r = renderer();
	for (auto actor : impl->m_imageActors) {
		r->RemoveActor2D(actor);
		actor->Delete();
	}
	impl->m_imageActors.clear();
	impl->m_pointsActor->VisibilityOff();
	impl->m_editTargetPointActor->VisibilityOff();

	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = points->impl->m_displaySetting;
	}

	// The point selected in pre-processor is removed from m_pointsPolyData and data(), and held as edit target data,
	// so it should be drawn separately.
	auto target = dynamic_cast<GeoDataPoint*> (points->editTargetData());
	if (target != nullptr && ! target->isDefined()) {
		target = nullptr;
	}

	if (ds.shape == GeoDataPointGroup::DisplaySetting::Shape::Point || ds.image.isNull()) {
		auto cm = colorMapSettingContainer();

		Impl::setupPointsActor(impl->m_pointsActor, points->impl->m_pointsPolyData, ds, cm);
		actorCollection()->AddItem(impl->m_pointsActor);

		if (target != nullptr) {
			Impl::setupPointsActor(impl->m_editTargetPointActor, target->impl->m_pointController.polyData(), ds, cm);
			actorCollection()->AddItem(impl->m_editTargetPointActor);
		}
	} else {
		auto view = dynamic_cast<VTK2DGraphicsView*> (dataModel()->graphicsView());
		auto pixmap = QPixmap::fromImage(ds.image);
		auto shrinkedPixmap = GeoDataPointGroup::Impl::shrinkPixmap(pixmap, ds, view);
		impl->m_shrinkedImage = shrinkedPixmap.toImage();
		auto imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
		imgToImg->SetQImage(&impl->m_shrinkedImage);
		auto imageInfo = vtkSmartPointer<vtkImageChangeInformation>::New();
		imageInfo->SetInputConnection(imgToImg->GetOutputPort());
		imageInfo->CenterImageOn();
		auto col = actor2DCollection();
		const auto& data = points->data();

		std::vector<QPointF> positions;
		for (auto it = data.rbegin(); it != data.rend(); ++it) {
			auto point = dynamic_cast<GeoDataPointGroupPoint*> (*it);
			positions.push_back(point->point());
		}
		if (target != nullptr) {
			positions.push_back(target->impl->m_pointController.point());
		}

		for (const auto& p : positions) {
			auto p2 = GeoDataPointGroup::Impl::buildBottomLeftCorner(p, shrinkedPixmap, ds.anchorPosition, view);

			auto mapper = vtkSmartPointer<vtkImageMapper>::New();
			mapper->SetColorWindow(255);
			mapper->SetColorLevel(127.5);
			mapper->SetInputConnection(imageInfo->GetOutputPort());

			auto actor = vtkActor2D::New();
			actor->SetMapper(mapper);
			actor->GetProperty()->SetOpacity(ds.opacity);

			auto coord = actor->GetPositionCoordinate();
			coord->SetCoordinateSystemToWorld();
			coord->SetValue(p2.x(), p2.y(), 0);

			r->AddActor2D(actor);
			col->AddItem(actor);
			impl->m_imageActors.push_back(actor);
		}
	}
	updateVisibilityWithoutRendering();
}

void GeoDataPointGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_pointsActor->SetPosition(0, 0, range.min());
	impl->m_editTargetPointActor->SetPosition(0, 0, range.min());
}

void GeoDataPointGroupProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataPointGroupProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
