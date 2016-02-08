#include "tmsimagegroupdataitem.h"
#include "tmsimagedataitem.h"
#include "../misc/targeted/targeteditemsettargetcommandtool.h"
#include "../named/namedgraphicswindowdataitemtool.h"

#include <guibase/coordinatesystem.h>
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
#include <QNetworkProxy>
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

void calcImageParameters(QPointF* center, QSize* size, QPointF* lowerLeft, double* scale, VTK2DGraphicsView* view, const CoordinateSystem& cs)
{
	double xmin, xmax, ymin, ymax;
	view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	QRectF rect(xmin, ymin, (xmax - xmin), (ymax - ymin));

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

TmsImageGroupDataItem::TmsImageGroupDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(tr("Background Images (Internet)"), QIcon(":/libs/guibase/images/iconFolder.png"), parent),
	m_tmsLoader {this},
	m_tmsRequestId {-1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

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

	renderer()->AddActor(m_actor);
	m_actorCollection->AddItem(m_actor);

	connect(&m_tmsLoader, SIGNAL(imageUpdated(int)), this, SLOT(handleImageUpdate(int)));

	rebuildChildItems();
}

TmsImageGroupDataItem::~TmsImageGroupDataItem()
{
	renderer()->RemoveActor(m_actor);
}

std::string TmsImageGroupDataItem::target() const
{
	return m_target;
}

void TmsImageGroupDataItem::setTarget(const std::string &target)
{
	if (target != "" && projectData()->mainfile()->coordinateSystem() == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To get background image from internet, please specify the coordiyate system of the project, from the following menu.\n File -> Property"));
		return;
	}

	bool ok = NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	if (ok) {
		m_target = target;
	} else {
		m_target = "";
	}

	if (m_target == ""){
		m_actor->VisibilityOff();
	}

	requestImage();
}

void TmsImageGroupDataItem::rebuildChildItems()
{
	clearChildItems();

	TmsImageSettingManager manager;
	for (TmsImageSetting s : manager.settings()) {
		if (! s.isActive()) {continue;}

		auto item = new TmsImageDataItem(s.setting(), s.caption(), this);
		m_childItems.append(item);
	}

	if (m_target == "") {return;}
	if (projectData()->mainfile()->coordinateSystem() == nullptr) {return;}

	setTarget(m_target);
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
	if (requestId != m_tmsRequestId) {return;}

	m_image = m_tmsLoader.getImage(m_tmsRequestId);
	// m_image.save("E:/debug.png"); // only for debug
	m_imgToImg->Modified();

	m_plane->SetPoint1(m_image.width(), 0, 0);
	m_plane->SetPoint2(0, m_image.height(), 0);
	m_plane->Modified();

	double pos[3];
	m_actor->GetPosition(pos);
	pos[0] = m_imageLowerLeft.x();
	pos[1] = m_imageLowerLeft.y();
	m_actor->SetPosition(pos);

	m_actor->SetScale(m_imageScale);
	m_actor->SetOrientation(0, 0, 0);
	m_actor->VisibilityOn();

	renderGraphicsView();
}

void TmsImageGroupDataItem::requestImage()
{
	if (m_tmsRequestId != -1) {
		m_tmsLoader.cancelRequest(m_tmsRequestId);
	}
	m_tmsLoader.setProxy(iricMainWindow()->networkProxy());

	auto view = dynamic_cast<VTK2DGraphicsView*> (dataModel()->graphicsView());
	if (view == nullptr) {return;}

	auto cs = projectData()->mainfile()->coordinateSystem();
	if (cs == nullptr) {return;}

	QPointF center;
	QSize size;

	calcImageParameters(&center, &size, &m_imageLowerLeft, &m_imageScale, view, *cs);

	TmsImageSettingManager manager;
	tmsloader::TmsRequest* request = manager.buildRequest(center, size, m_imageScale, m_target);
	if (request == nullptr) {return;}

	m_tmsLoader.registerRequest(*request, &m_tmsRequestId);

	delete request;
}

void TmsImageGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	double pos[3];
	m_actor->GetPosition(pos);
	pos[2] = range.min();

	m_actor->SetPosition(pos);
}

void TmsImageGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto target = iRIC::toStr(node.toElement().attribute("target"));
	setTarget(target);
}

void TmsImageGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("target", m_target.c_str());
}
