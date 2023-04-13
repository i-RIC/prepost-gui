#include "geodatapoint.h"
#include "geodatapointproxy.h"

#include "private/geodatapoint_impl.h"
#include "private/geodatapointproxy_displaysettingwidget.h"
#include "private/geodatapointproxy_impl.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/pre/geodata/private/geodataproxy_propertydialog.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <misc/zdepthrange.h>

#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkQImageToImageSource.h>

GeoDataPointProxy::GeoDataPointProxy(GeoDataPoint* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataPointProxy::~GeoDataPointProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_pointActor);
	r->RemoveActor2D(impl->m_imageActor);

	delete impl;
}

void GeoDataPointProxy::setupActors()
{
	auto r = renderer();

	r->AddActor(impl->m_pointActor);
	r->AddActor2D(impl->m_imageActor);

	updateActorSetting();
}

void GeoDataPointProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPointProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPointProxy::propertyDialog(QWidget* parent)
{
	auto dialog = new PropertyDialog(this, parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle("Point Display Setting");
	return dialog;
}

void GeoDataPointProxy::updateActorSetting()
{
	auto point = dynamic_cast<GeoDataPoint*> (geoData());
	auto r = renderer();
	impl->m_pointActor->VisibilityOff();
	impl->m_imageActor->VisibilityOff();

	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = point->impl->m_displaySetting;
	}

	if (ds.shape == GeoDataPoint::DisplaySetting::Shape::Point || ds.image.isNull()) {
		// color
		impl->m_pointActor->GetProperty()->SetColor(ds.color);

		// mapping
		auto cm = colorMapSettingContainer();
		vtkMapper* mapper = nullptr;
		if (ds.mapping == GeoDataPoint::DisplaySetting::Mapping::Value && (cm != nullptr)) {
			mapper = cm->buildCellDataMapper(point->impl->m_pointController.polyData(), true);
		} else {
			auto polyDataMapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
			polyDataMapper->SetInputData(point->impl->m_pointController.polyData());
			impl->m_pointActor->SetMapper(mapper);
			mapper->Delete();
		}
		impl->m_pointActor->SetMapper(mapper);
		mapper->Delete();

		// opacity
		impl->m_pointActor->GetProperty()->SetOpacity(ds.opacity);

		// pointSize
		impl->m_pointActor->GetProperty()->SetPointSize(ds.pointSize);

		actorCollection()->AddItem(impl->m_pointActor);
	} else {
		auto pixmap = QPixmap::fromImage(ds.image);
		impl->m_shrinkedImage = GeoDataPoint::Impl::shrinkPixmap(pixmap, ds.imageMaxSize).toImage();
		auto imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
		imgToImg->SetQImage(&impl->m_shrinkedImage);
		auto imageInfo = vtkSmartPointer<vtkImageChangeInformation>::New();
		imageInfo->SetInputConnection(imgToImg->GetOutputPort());
		imageInfo->CenterImageOn();
		auto col = actor2DCollection();

		auto mapper = vtkSmartPointer<vtkImageMapper>::New();
		mapper->SetColorWindow(255);
		mapper->SetColorLevel(127.5);
		mapper->SetInputConnection(imageInfo->GetOutputPort());

		impl->m_imageActor->SetMapper(mapper);
		impl->m_imageActor->GetProperty()->SetOpacity(ds.opacity);

		auto coord = impl->m_imageActor->GetPositionCoordinate();
		coord->SetCoordinateSystemToWorld();
		auto p = point->impl->m_pointController.point();
		coord->SetValue(p.x(), p.y(), 0);

		actor2DCollection()->AddItem(impl->m_imageActor);
	}
	updateVisibilityWithoutRendering();
}

void GeoDataPointProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_pointActor->SetPosition(0, 0, range.min());
}

void GeoDataPointProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataPointProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
