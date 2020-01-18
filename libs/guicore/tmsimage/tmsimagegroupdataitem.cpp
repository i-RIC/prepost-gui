#include "tmsimagegroupdataitem.h"
#include "tmsimagedataitem.h"
#include "private/tmsimagegroupdataitem_impl.h"

#include "../misc/targeted/targeteditemsettargetcommandtool.h"
#include "../named/namedgraphicswindowdataitemtool.h"

#include <cs/coordinatesystem.h>
#include <cs/webmercatorutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/tmsimage/tmsimagesetting.h>
#include <guicore/tmsimage/tmsimagesettingmanager.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
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

QRectF calcRect(VTK2DGraphicsView* view, const QPointF& offset)
{
	double xmin, xmax, ymin, ymax;

	view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
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

	view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
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

	double newWidth = std::abs(std::cos(angleRad)) * windowSize.width() + std::abs(std::sin(angleRad)) * windowSize.height();
	double newHeight = std::abs(std::sin(angleRad)) * windowSize.width() + std::abs(std::cos(angleRad)) * windowSize.height();

	*size = QSize(newWidth, newHeight);
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

void calcImageParametersLonLat(QPointF* center, QSize* size, double* requestScale, QPointF* lowerLeft, double* scale, VTK2DGraphicsView* view, const QPointF& offset)
{
	QRectF rect = calcRect(view, offset);
	double orig_width = rect.width();

	rect = trimLonLatRect(rect);
	double new_width = rect.width();
	*lowerLeft = QPointF(rect.left() - offset.x(), rect.top() - offset.y());

	QPointF vecX = calcVecX(view);
	QPointF stdVecX(1, 0);
	double angleRad = iRIC::angleRadian(stdVecX, vecX);
	QSize windowSize = view->size();

	double newWidth = std::abs(std::cos(angleRad)) * windowSize.width() + std::abs(std::sin(angleRad)) * windowSize.height();
	newWidth *= new_width / orig_width;

	double lonCenter, latCenter;
	int zoomLevel, width, height;
	WebMercatorUtil::calcImageZoomAndSize(rect.left(), rect.top(), rect.right(), rect.bottom(), newWidth,
																				&lonCenter, &latCenter, &zoomLevel, &width, &height);

	*center = QPointF(lonCenter, latCenter);
	*requestScale = tmsloader::TmsUtil::meterPerPixel(*center, zoomLevel);
	*size = QSize(width, height);

	*scale = iRIC::length(vecX) * (newWidth / width);
}

void calcImageParameters(QPointF* center, QSize* size, double* requestScale, QPointF* lowerLeft, double* scale, VTK2DGraphicsView* view, const CoordinateSystem& cs, const QPointF& offset)
{
	if (cs.isLongLat()) {
		calcImageParametersLonLat(center, size, requestScale, lowerLeft, scale, view, offset);
	} else {
		calcImageParameters(center, size, lowerLeft, scale, view, cs, offset);
		*requestScale = *scale;
	}
}

} // namespace

TmsImageGroupDataItem::Impl::Impl(TmsImageGroupDataItem *parent) :
	m_tmsLoader {parent->iricMainWindow()},
	m_tmsRequestId {-1},
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
	GraphicsWindowDataItem(tr("Background Images (Internet)"), QIcon(":/libs/guibase/images/iconFolder.png"), parent),
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

	delete impl;
}

std::string TmsImageGroupDataItem::target() const
{
	return impl->m_target;
}

void TmsImageGroupDataItem::setTarget(const std::string &target)
{
	if (target != "" && projectData()->mainfile()->coordinateSystem() == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To get background image from internet, please specify the coordiyate system of the project, from the following menu.\n File -> Property"));
		return;
	}

	bool ok = NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	if (ok) {
		impl->m_target = target;
	} else {
		impl->m_target = "";
	}

	if (impl->m_target == ""){
		impl->m_actor->VisibilityOff();
	}

	requestImage();
}

void TmsImageGroupDataItem::rebuildChildItems()
{
	auto activeItem = dataModel()->graphicsView()->activeDataItem();
	for (auto item : childItems()) {
		if (item == activeItem) {
			dataModel()->graphicsView()->setActiveDataItem(nullptr);
		}
	}
	clearChildItems();
	updateItemMap();

	TmsImageSettingManager manager;
	for (TmsImageSetting s : manager.settings()) {
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

void TmsImageGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void TmsImageGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Background Image change"));
	pushRenderCommand(cmd, this, true);
}

void TmsImageGroupDataItem::handleImageUpdate(int requestId)
{
	if (requestId != impl->m_tmsRequestId) {return;}

	auto cs = projectData()->mainfile()->coordinateSystem();
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
	updateVisibility();
}

void TmsImageGroupDataItem::requestImage()
{
	if (impl->m_tmsRequestId != -1) {
		impl->m_tmsLoader.cancelRequest(impl->m_tmsRequestId);
	}
	auto view = dynamic_cast<VTK2DGraphicsView*> (dataModel()->graphicsView());
	if (view == nullptr) {return;}

	auto cs = projectData()->mainfile()->coordinateSystem();
	if (cs == nullptr) {return;}

	QPointF center;
	QSize size;
	double scale;

	calcImageParameters(&center, &size, &scale, &(impl->m_imageLowerLeft), &(impl->m_imageScale), view, *cs, impl->m_offset);

	TmsImageSettingManager manager;
	tmsloader::TmsRequest* request = manager.buildRequest(center, size, scale, impl->m_target);
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

void TmsImageGroupDataItem::doHandleResize(VTKGraphicsView*)
{
	requestImage();
}

void TmsImageGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_offset.setX(offset().x());
	impl->m_offset.setY(offset().y());
	auto target = iRIC::toStr(node.toElement().attribute("target"));
	setTarget(target);
}

void TmsImageGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("target", impl->m_target.c_str());
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
