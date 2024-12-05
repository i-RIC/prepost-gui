#include "tmsimagegroupdataitem.h"
#include "tmsimagedataitem.h"
#include "private/tmsimagegroupdataitem_impl.h"

#include "../misc/targeted/targetedqstringitemsettargetcommandtool.h"
#include "../named/namedqstringgraphicswindowdataitemtool.h"

#include <cs/coordinatesystem.h>
#include <cs/webmercatorutil.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/project/projectworkspace.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/tmsimage/tmsimagesetting.h>
#include <guicore/tmsimage/tmsimagesettingmanager.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/valuechangert.h>
#include <tmsloader/tmsrequest.h>
#include <tmsloader/tmsutil.h>

#include <QDomNode>
#include <QMessageBox>
#include <QRectF>
#include <QXmlStreamWriter>

#include <vtkRenderer.h>
#include <vtkTextureMapToPlane.h>
#include <vtkPolyDataMapper.h>

#include <cmath>

namespace {

	const double MARGIN_RATIO = 0.8;

void getExtendedDrawnRegion(VTK2DGraphicsView* view, double* xmin, double* xmax, double* ymin, double* ymax)
{
	view->getDrawnRegion(xmin, xmax, ymin, ymax);
	double width = (*xmax - *xmin);
	double height = (*ymax - *ymin);
	*xmin -= width * MARGIN_RATIO;
	*xmax += width * MARGIN_RATIO;
	*ymin -= height * MARGIN_RATIO;
	*ymax += height * MARGIN_RATIO;
}

QRectF calcRect(VTK2DGraphicsView* view, const QPointF& offset)
{
	double xmin, xmax, ymin, ymax;

	getExtendedDrawnRegion(view, &xmin, &xmax, &ymin, &ymax);
	QRectF rect(xmin, ymin, (xmax - xmin), (ymax - ymin));
	rect.adjust(offset.x(), offset.y(), offset.x(), offset.y());

	return rect;
}

QPointF calcVecX(VTK2DGraphicsView* view)
{
	double x, y;

	x = 0;
	y = 0;
	view->viewportToWorld(x, y);
	QPointF origin(x, y);

	x = 1;
	y = 0;
	view->viewportToWorld(x, y);
	QPointF p1(x, y);

	return p1 - origin;
}

void calcImageParameters(QPointF* center, QSize* size, QPointF* lowerLeft, double* scale, VTK2DGraphicsView* view, const CoordinateSystem& cs, const QPointF& offset)
{
	double xmin, xmax, ymin, ymax;
	double centerX, centerY, centerLongitude, centerLatitude;

	getExtendedDrawnRegion(view, &xmin, &xmax, &ymin, &ymax);
	*lowerLeft = QPointF(xmin, ymin);

	QRectF rect = calcRect(view, offset);
	centerX = (rect.left() + rect.right()) * 0.5;
	centerY = (rect.top() + rect.bottom()) * 0.5;

	cs.mapGridToGeo(centerX, centerY, &centerLongitude, &centerLatitude);
	*center = QPointF(centerLongitude, centerLatitude);

	QPointF vecX = calcVecX(view);
	*scale = iRIC::length(vecX);
	QPointF stdVecX(1, 0);

	double angleRad = iRIC::angleRadian(stdVecX, vecX);
	QSize windowSize = view->size();

	double newWidth = std::abs(std::cos(angleRad)) * windowSize.width() * view->devicePixelRatioF() + std::abs(std::sin(angleRad)) * windowSize.height() * view->devicePixelRatioF();
	double newHeight = std::abs(std::sin(angleRad)) * windowSize.width() * view->devicePixelRatioF() + std::abs(std::cos(angleRad)) * windowSize.height() * view->devicePixelRatioF();

	*size = QSize(newWidth * (1. + 2 * MARGIN_RATIO), newHeight * (1. + 2 * MARGIN_RATIO));
}

QRectF trimLonLatRect(const QRectF& rect)
{
	QRectF ret = rect;
	if (ret.left()   < -180) {ret.setLeft (-180);}
	if (ret.right()  >  180) {ret.setRight( 180);}
	if (ret.top()    <  -89) {ret.setTop(   -89);}
	if (ret.bottom() >   89) {ret.setBottom( 89);}
	return ret;
}

void calcSizeAndZoomLevel(const QSize& targetSize, double targetMeterPerPixel, const QPointF& center, QSize* size, int* zoomLevel, double* ratio)
{
	*zoomLevel = tmsloader::TmsUtil::calcNativeZoomLevel(center, targetMeterPerPixel) + 1;
	while (true) {
		-- *zoomLevel;
		double mpp = tmsloader::TmsUtil::meterPerPixel(center, *zoomLevel);
		*ratio = targetMeterPerPixel / mpp;
		*size = QSize(targetSize.width() * *ratio, targetSize.height() * *ratio);

		if (*ratio < 1.0) {break;}
	}
}

void calcRequestParameters(QPointF* center, QSize* size, double* scale, QPointF* lowerLeft, int* zoomLevel, VTK2DGraphicsView* view, const CoordinateSystem& cs, const QPointF& offset)
{
	if (cs.isLongLat()) {
		QRectF rect = calcRect(view, offset);
		double orig_width = rect.width();

		rect = trimLonLatRect(rect);
		double new_width = rect.width();
		*lowerLeft = QPointF(rect.left() - offset.x(), rect.top() - offset.y());

		QPointF vecX = calcVecX(view);
		QPointF stdVecX(1, 0);
		double angleRad = iRIC::angleRadian(stdVecX, vecX);
		QSize windowSize = view->size();

		double newWidth = std::abs(std::cos(angleRad)) * windowSize.width() * view->devicePixelRatioF() + std::abs(std::sin(angleRad)) * windowSize.height() * view->devicePixelRatioF();
		newWidth *= new_width / orig_width;

		double lonCenter, latCenter;
		int width, height;
		WebMercatorUtil::calcImageZoomAndSize(rect.left(), rect.top(), rect.right(), rect.bottom(), newWidth * (1. + 2 * MARGIN_RATIO),
																					&lonCenter, &latCenter, zoomLevel, &width, &height);
		*center = QPointF(lonCenter, latCenter);
		*size = QSize(width, height);
		*scale = iRIC::length(vecX) * (newWidth * (1. + 2 * MARGIN_RATIO) / width);
	} else {
		QSize screenSize;
		double ratio;
		calcImageParameters(center, &screenSize, lowerLeft, scale, view, cs, offset);
		calcSizeAndZoomLevel(screenSize, *scale, *center, size, zoomLevel, &ratio);
		*scale /= ratio;
	}
}

} // namespace

TmsImageGroupDataItem::Impl::Impl(TmsImageGroupDataItem *parent) :
	m_tmsLoader {parent->iricMainWindow()},
	m_tmsRequestId {-1},
	m_actorIsVisible {false},
	m_offset {parent->offset().x(), parent->offset().y()},
	m_parent {parent}
{
	m_texture = vtkSmartPointer<vtkTexture>::New();
	m_texture->InterpolateOn();

	m_plane = vtkSmartPointer<vtkPlaneSource>::New();
	m_plane->SetPoint1(m_image.width(), 0, 0);
	m_plane->SetPoint2(0, m_image.height(), 0);

	vtkSmartPointer<vtkTextureMapToPlane> map = vtkSmartPointer<vtkTextureMapToPlane>::New();
	map->SetInputConnection(m_plane->GetOutputPort());
	map->AutomaticPlaneGenerationOn();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(map->GetOutputPort());

	m_imgToImg = vtkSmartPointer<vtkQImageToImageSource>::New();
	m_imgToImg->SetQImage(&m_image);
	m_texture->SetInputConnection(m_imgToImg->GetOutputPort());

	m_actor = vtkSmartPointer<vtkActor>::New();
	m_actor->SetMapper(mapper);
	m_actor->SetTexture(m_texture);
	m_actor->VisibilityOff();
}

TmsImageGroupDataItem::TmsImageGroupDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(tr("Background Images (Internet)"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent),
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	renderer()->AddActor(impl->m_actor);
	m_actorCollection->AddItem(impl->m_actor);

	connect(&impl->m_tmsLoader, SIGNAL(imageUpdated(int)), this, SLOT(handleImageUpdate(int)));

	rebuildChildItems();
}

TmsImageGroupDataItem::~TmsImageGroupDataItem()
{
	renderer()->RemoveActor(impl->m_actor);
}

QString TmsImageGroupDataItem::target() const
{
	return impl->m_target;
}

void TmsImageGroupDataItem::setTarget(const QString &target)
{
	if (target != "" && projectData()->mainfile()->coordinateSystem() == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To get background image from internet, please specify the coordiyate system of the project, from the following menu.\n File -> Property"));
		return;
	}

	bool ok = NamedQStringGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	if (ok) {
		impl->m_target = target;
	} else {
		impl->m_target = "";
	}

	if (impl->m_target == ""){
		impl->m_actor->VisibilityOff();
	}

	requestImage(true);
}

void TmsImageGroupDataItem::rebuildChildItems()
{
	auto activeItem = dataModel()->graphicsView()->activeDataItem();
	for (auto item : childItems()) {
		if (item == activeItem) {
			dataModel()->graphicsView()->setActiveDataItem(nullptr);
		}
	}
	clearChildItemsAndUpdateItemMap();

	TmsImageSettingManager manager;
	for (const auto& s : manager.settings()) {
		if (! s.isActive()) {continue;}
		auto item = new TmsImageDataItem(s.setting(), s.caption(), this);
		m_childItems.push_back(item);
	}
	updateItemMap();

	if (impl->m_target == "") {return;}
	if (projectData()->mainfile()->coordinateSystem() == nullptr) {return;}

	setTarget(impl->m_target);
}

void TmsImageGroupDataItem::viewOperationEndedGlobal(VTKGraphicsView*)
{
	requestImage();
}

void TmsImageGroupDataItem::disableActor()
{
	impl->m_actorIsVisible = impl->m_actor->GetVisibility();
	impl->m_actor->VisibilityOff();
}

void TmsImageGroupDataItem::restoreActor()
{
	if (impl->m_actorIsVisible) {
		impl->m_actor->VisibilityOn();
	}
}

void TmsImageGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void TmsImageGroupDataItem::handleNamedItemChange(NamedQStringGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedQStringItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Background Image change"));
	pushRenderCommand(cmd, this, true);
}

void TmsImageGroupDataItem::handleImageUpdate(int requestId)
{
	if (requestId != impl->m_tmsRequestId) {return;}

	auto cs = projectData()->mainfile()->coordinateSystem();
	if (cs == nullptr) {return;}

	if (cs->isLongLat()) {
		auto view = dynamic_cast<VTK2DGraphicsView*> (dataModel()->graphicsView());
		QRectF rect = calcRect(view, impl->m_offset);
		rect = trimLonLatRect(rect);
		impl->m_image = WebMercatorUtil::convertWebMercatorToLongLat(rect, impl->m_tmsLoader.getImage(impl->m_tmsRequestId), projectData()->workDirectory());
	} else {
		impl->m_image = impl->m_tmsLoader.getImage(impl->m_tmsRequestId);
	}

	// m_image.save("E:/debug.png"); // only for debug
	impl->m_imgToImg->Modified();

	impl->m_plane->SetPoint1(impl->m_image.width(), 0, 0);
	impl->m_plane->SetPoint2(0, impl->m_image.height(), 0);
	impl->m_plane->Modified();

	double pos[3];
	impl->m_actor->GetPosition(pos);
	pos[0] = impl->m_imageLowerLeft.x();
	pos[1] = impl->m_imageLowerLeft.y();
	impl->m_actor->SetPosition(pos);

	impl->m_actor->SetScale(impl->m_imageScale);
	impl->m_actor->SetOrientation(0, 0, 0);

	auto view = dynamic_cast<VTK2DGraphicsView*> (dataModel()->graphicsView());
	view->ResetCameraClippingRange();

	updateVisibility();
}

void TmsImageGroupDataItem::requestImage(bool force)
{
	static bool requesting = false;

	if (requesting) {return;}
	ValueChangerT<bool> changer(&requesting, true);

	auto view = dynamic_cast<VTK2DGraphicsView*> (dataModel()->graphicsView());
	if (view == nullptr) {return;}

	auto cs = projectData()->mainfile()->coordinateSystem();
	if (cs == nullptr) {return;}

	// skip reload image if four corners are inside image
	double xmin = impl->m_imageLowerLeft.x();
	double ymin = impl->m_imageLowerLeft.y();
	double xmax = xmin + impl->m_imageScale * impl->m_image.width();
	double ymax = ymin + impl->m_imageScale * impl->m_image.height();

	bool all_ok = ! force;

	auto tl = view->viewportToWorld(QPoint(0, 0));
	all_ok = all_ok && (tl.x() >= xmin && tl.x() <= xmax && tl.y() >= ymin && tl.y() <= ymax);
	auto tr = view->viewportToWorld(QPoint(view->width(), 0));
	all_ok = all_ok && (tr.x() >= xmin && tr.x() <= xmax && tr.y() >= ymin && tr.y() <= ymax);
	auto ll = view->viewportToWorld(QPoint(0, view->height()));
	all_ok = all_ok && (ll.x() >= xmin && ll.x() <= xmax && ll.y() >= ymin && ll.y() <= ymax);
	auto lr = view->viewportToWorld(QPoint(view->width(), view->height()));
	all_ok = all_ok && (lr.x() >= xmin && lr.x() <= xmax && lr.y() >= ymin && lr.y() <= ymax);

	QPointF center;
	QSize size;
	double imageScale;
	QPointF imageLowerLeft;
	int zoomLevel;

	double oldImageScale = impl->m_imageScale;
	calcRequestParameters(&center, &size, &imageScale, &imageLowerLeft, &zoomLevel, view, *cs, impl->m_offset);

/*
	QPointF center;
	QSize size;
	double scale;
	QPointF imageLowerLeft;
	double imageScale;

	double oldImageScale = impl->m_imageScale;
	calcImageParameters(&center, &size, &scale, &imageLowerLeft, &imageScale, view, *cs, impl->m_offset);

	QSize imageSize;
	double ratio;
	int zoomLevel;
	calcSizeAndZoomLevel(size, scale, center, &imageSize, &zoomLevel, &ratio);

	imageScale /= ratio;
	*/

	double r = imageScale / oldImageScale;
	all_ok = all_ok && (r < 1.2 && 1.0 / r < 1.2);

	if (all_ok) {return;}

	impl->m_imageLowerLeft = imageLowerLeft;
	impl->m_imageScale = imageScale;

	if (impl->m_tmsRequestId != -1) {
		impl->m_tmsLoader.cancelRequest(impl->m_tmsRequestId);
	}

	TmsImageSettingManager manager;
	auto setting = TmsImageSetting::buildFromString(impl->m_target);
	tmsloader::TmsRequest* request = manager.buildRequest(center, size, zoomLevel, setting);
	if (request == nullptr) {return;}

	impl->m_tmsLoader.registerRequest(*request, &(impl->m_tmsRequestId));

	delete request;
}

void TmsImageGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	double pos[3];
	impl->m_actor->GetPosition(pos);
	pos[2] = range.min();

	impl->m_actor->SetPosition(pos);
}

void TmsImageGroupDataItem::doHandleResize(QResizeEvent*, VTKGraphicsView*)
{
	requestImage();
}

void TmsImageGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_offset.setX(offset().x());
	impl->m_offset.setY(offset().y());
	auto target = node.toElement().attribute("target");
	setTarget(target);
}

void TmsImageGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("target", impl->m_target);
}

void TmsImageGroupDataItem::doApplyOffset(double x_diff, double y_diff)
{
	setIsCommandExecuting(true);
	double x = x_diff + impl->m_offset.x();
	double y = y_diff + impl->m_offset.y();
	impl->m_offset.setX(x);
	impl->m_offset.setY(y);
	// force update
	setTarget(target());
	setIsCommandExecuting(false);
}
