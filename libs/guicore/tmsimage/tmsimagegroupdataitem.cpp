#include "tmsimagegroupdataitem.h"
#include "tmsimagedataitem.h"
#include "private/tmsimagegroupdataitem_impl.h"

#include "../misc/targeted/targeteditemsettargetcommandtool.h"
#include "../named/namedgraphicswindowdataitemtool.h"

#include <cs/coordinatesystem.h>
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

#include <QDomNode>
#include <QMessageBox>
#include <QRectF>
#include <QXmlStreamWriter>

#include <vtkRenderer.h>
#include <vtkTextureMapToPlane.h>
#include <vtkPolyDataMapper.h>

#include <cmath>

namespace {

void calcImageParameters(QPointF* center, QSize* size, double* scale, const QRectF& rect, const QVector2D& vecX, const QSize& windowSize, const CoordinateSystem& cs)
{
	double centerX, centerY, centerLongitude, centerLatitude;

	centerX = (rect.left() + rect.right()) * 0.5;
	centerY = (rect.top() + rect.bottom()) * 0.5;

	cs.mapGridToGeo(centerX, centerY, &centerLongitude, &centerLatitude);

	*center = QPointF(centerLongitude, centerLatitude);

	*scale = vecX.length();

	QVector2D stdVecX(1, 0);

	double angleRad = iRIC::angleRadian(stdVecX, vecX);

	double newWidth = std::abs(std::cos(angleRad)) * windowSize.width() + std::abs(std::sin(angleRad)) * windowSize.height();
	double newHeight = std::abs(std::sin(angleRad)) * windowSize.width() + std::abs(std::cos(angleRad)) * windowSize.height();

	*size = QSize(newWidth, newHeight);
}

void calcImageParameters(QPointF* center, QSize* size, QPointF* lowerLeft, double* scale, VTK2DGraphicsView* view, const CoordinateSystem& cs, const QPointF& offset)
{
	double xmin, xmax, ymin, ymax;
	view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	QRectF rect(xmin, ymin, (xmax - xmin), (ymax - ymin));
	rect.adjust(offset.x(), offset.y(), offset.x(), offset.y());

	double x, y;

	x = 0;
	y = 0;
	view->viewportToWorld(x, y);
	QVector2D vecOrigin(x, y);

	x = 1;
	y = 0;
	view->viewportToWorld(x, y);
	QVector2D vecX(x, y);

	*lowerLeft = QPointF(xmin, ymin);

	QSize windowSize = view->size();

	calcImageParameters(center, size, scale, rect, vecX - vecOrigin, windowSize, cs);
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

	impl->m_image = impl->m_tmsLoader.getImage(impl->m_tmsRequestId);
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

	calcImageParameters(&center, &size, &(impl->m_imageLowerLeft), &(impl->m_imageScale), view, *cs, impl->m_offset);

	TmsImageSettingManager manager;
	tmsloader::TmsRequest* request = manager.buildRequest(center, size, impl->m_imageScale, impl->m_target);
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
