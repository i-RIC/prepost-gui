#include "geodatapointgroup.h"
#include "geodatapointgrouppoint.h"
#include "geodatapointgroupproxy.h"
#include "geodatapointgroupcolorsettingdialog.h"

#include "private/geodatapointgroup_impl.h"
#include "private/geodatapointgroupproxy_displaysettingwidget.h"
#include "private/geodatapointgroupproxy_impl.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/pre/geodata/private/geodataproxy_propertydialog.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <misc/zdepthrange.h>

#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageMapper.h>
#include <vtkQImageToImageSource.h>
#include <vtkProperty2D.h>

GeoDataPointGroupProxy::GeoDataPointGroupProxy(GeoDataPointGroup* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataPointGroupProxy::~GeoDataPointGroupProxy()
{
	auto r = renderer();

	for (auto actor : impl->m_imageActors) {
		r->RemoveActor2D(actor);
	}
	r->RemoveActor(impl->m_pointsActor);

	delete impl;
}

void GeoDataPointGroupProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_pointsActor);
	col->AddItem(impl->m_pointsActor);

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
	dialog->setWindowTitle("Points Display Setting");
	return dialog;
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

	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = points->impl->m_displaySetting;
	}

	if (ds.shape == GeoDataPointGroup::DisplaySetting::Shape::Point || ds.image.isNull()) {
		// color
		QColor c = ds.color;

		impl->m_pointsActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());

		// mapping
		bool scalarVisibility = true;
		if (ds.mapping == GeoDataPointGroup::DisplaySetting::Mapping::Arbitrary) {
			scalarVisibility = false;
		}
		if (scalarVisibility) {
			vtkMapper* mapper = nullptr;
			auto cs = colorMapSettingContainer();

			mapper = cs->buildCellDataMapper(points->impl->m_pointsPolyData, true);
			impl->m_pointsActor->SetMapper(mapper);
			mapper->Delete();
		} else {
			vtkPolyDataMapper* mapper = nullptr;

			mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
			mapper->SetInputData(points->impl->m_pointsPolyData);
			impl->m_pointsActor->SetMapper(mapper);
			mapper->Delete();
		}

		// opacity
		impl->m_pointsActor->GetProperty()->SetOpacity(ds.opacity);

		// pointSize
		impl->m_pointsActor->GetProperty()->SetPointSize(ds.pointSize);

		actorCollection()->AddItem(impl->m_pointsActor);
	} else {
		auto pixmap = QPixmap::fromImage(ds.image);
		impl->m_shrinkedImage = GeoDataPointGroup::Impl::shrinkPixmap(pixmap, ds.imageMaxSize).toImage();
		auto imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
		imgToImg->SetQImage(&impl->m_shrinkedImage);
		auto imageInfo = vtkSmartPointer<vtkImageChangeInformation>::New();
		imageInfo->SetInputConnection(imgToImg->GetOutputPort());
		imageInfo->CenterImageOn();
		auto col = actor2DCollection();

		const auto& data = points->data();
		for (auto it =  data.rbegin(); it != data.rend(); ++it) {
			auto point = dynamic_cast<GeoDataPointGroupPoint*> (*it);
			auto p = point->point();
			auto mapper = vtkSmartPointer<vtkImageMapper>::New();
			mapper->SetColorWindow(255);
			mapper->SetColorLevel(127.5);
			mapper->SetInputConnection(imageInfo->GetOutputPort());

			auto actor = vtkActor2D::New();
			actor->SetMapper(mapper);
			actor->GetProperty()->SetOpacity(ds.opacity);

			auto coord = actor->GetPositionCoordinate();
			coord->SetCoordinateSystemToWorld();
			coord->SetValue(p.x(), p.y(), 0);

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
