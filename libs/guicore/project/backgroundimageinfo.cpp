#define _USE_MATH_DEFINES

#include "backgroundimageinfo.h"
//#include "main/iricmainwindow.h"
#include "../datamodel/graphicswindowdataitem.h"
//#include "project/projectmainfile.h"
#include "backgroundimageinfodialog.h"
#include "../pre/base/preprocessorwindowinterface.h"
#include "../pre/base/preprocessordatamodelinterface.h"
#include "../datamodel/vtkgraphicsview.h"
#include "../datamodel/vtk2dgraphicsview.h"
#include "../pre/base/preprocessorgraphicsviewinterface.h"
#include "private/backgroundimageinfo_setactorpropertycommand.h"

#include "../base/iricmainwindowinterface.h"
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>

#include <vtkImageReader2.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkTIFFReader.h>
#include <vtkExtractVOI.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkMapperCollection.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkTextureMapToPlane.h>

#include <QMdiSubWindow>
#include <QFileInfo>
#include <QDir>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
#include <QMouseEvent>
#include <QDomElement>
#include <QXmlStreamWriter>

const int BackgroundImageInfo::MAXWIDTH = 2048;

BackgroundImageInfo::BackgroundImageInfo(const QString& filename, const QString& origFilename, ProjectDataItem* d)
	: ProjectDataItem(d)
{
	QFileInfo finfo(filename);
	m_name = filename;
	m_filename = finfo.fileName();
	m_caption = finfo.fileName();

	m_isRotating = false;
	m_isZooming = false;
	m_isTranslating = false;
	m_isMoving = false;

	m_fixed = false;

	m_hasWorldFile = false;
	m_angle = 0;
	m_scale = 1;
	m_translateX = -offset().x();
	m_translateY = -offset().y();
	m_visible = true;

	m_imageWidth = 0;
	m_imageHeight = 0;
	m_resizeScale = 1;

	m_movePixmap = QPixmap(":/libs/guicore/images/cursorImageMove.png");
	m_rotatePixmap = QPixmap(":/libs/guicore/images/cursorImageRotate.png");
	m_zoomPixmap = QPixmap(":/libs/guicore/images/cursorImageZoom.png");
	m_moveCursor = QCursor(m_movePixmap);
	m_rotateCursor = QCursor(m_rotatePixmap);
	m_zoomCursor = QCursor(m_zoomPixmap);

	m_fixAction = new QAction(tr("Fix Image position"), this);
	m_fixAction->setCheckable(true);
	m_fixActionWithIcon = new QAction(tr("Fix Image position"), this);
	connect(m_fixAction, SIGNAL(triggered()), this, SLOT(toggleFixState()));
	connect(m_fixActionWithIcon, SIGNAL(triggered()), this, SLOT(toggleFixState()));
	updateFixActionIcon();
	readImageData(filename, origFilename);

	m_refActor = vtkSmartPointer<vtkActor>::New();
	setupActor(m_refActor);
}

BackgroundImageInfo::~BackgroundImageInfo()
{
}

void BackgroundImageInfo::readImageData(const QString& filename, const QString& origFilename)
{
	QFileInfo finfo(origFilename);
	QDir dir = finfo.absoluteDir();
	vtkSmartPointer<vtkImageReader2> reader;

	QString ext = finfo.suffix().toLower();
	QString worldFilename = finfo.baseName();
	if (ext == "jpg" || ext == "jpeg") {
		// jpeg
		reader = vtkSmartPointer<vtkJPEGReader>::New();
		if (dir.exists(worldFilename.append(".jgw"))) {
			m_hasWorldFile = true;
			m_worldFilename = dir.absoluteFilePath(worldFilename);
		}
	} else if (ext == "png") {
		// png
		reader = vtkSmartPointer<vtkPNGReader>::New();
		if (dir.exists(worldFilename.append(".pgw"))) {
			m_hasWorldFile = true;
			m_worldFilename = dir.absoluteFilePath(worldFilename);
		}
	} else if (ext == "tif") {
		// tiff
		reader = vtkSmartPointer<vtkTIFFReader>::New();
		if (dir.exists(worldFilename.append(".tfw"))) {
			m_hasWorldFile = true;
			m_worldFilename = dir.absoluteFilePath(worldFilename);
		}
	}

	QPixmap pixmap;
	bool ok = pixmap.load(filename);
	if (! ok) {
		throw ErrorMessage(tr("Unable to read image %1").arg(finfo.fileName()));
	}
	bool scaled = false;

	m_imageWidth = pixmap.width();
	m_imageHeight = pixmap.height();
	m_resizeScale = 1;

	QSettings settings;

	int maxwidth = settings.value("graphics/backgroundimagemaxsize", MAXWIDTH).value<int>();

	if (pixmap.width() > pixmap.height()) {
		if (pixmap.width() > maxwidth) {
			m_resizeScale = static_cast<double>(maxwidth) / pixmap.width();
			pixmap = pixmap.scaledToWidth(maxwidth);
			scaled = true;
		}
	} else {
		if (pixmap.height() > maxwidth) {
			m_resizeScale = static_cast<double>(maxwidth) / pixmap.height();
			pixmap = pixmap.scaledToHeight(maxwidth);
			scaled = true;
		}
	}

	QString thumbnailName = getThumbnailFileName(filename);

	if (scaled) {
		// if scaled to smaller file, it is saved.
		pixmap.save(thumbnailName);
	} else {
		// thumbnail not needed. If it exists, remove it.
		QFile::remove(thumbnailName);
	}
	QFile thumb(thumbnailName);
	if (thumb.exists()) {
		reader = vtkSmartPointer<vtkJPEGReader>::New();
		reader->SetFileName(iRIC::toStr(thumbnailName).c_str());
	} else {
		reader->SetFileName(iRIC::toStr(filename).c_str());
	}
	reader->Update();
	vtkSmartPointer<vtkImageData> image = reader->GetOutput();

	m_texture = vtkSmartPointer<vtkTexture>::New();
	m_texture->SetInputData(image);
	m_texture->InterpolateOn();
	m_plane = vtkSmartPointer<vtkPlaneSource>::New();
	m_plane->SetCenter(0.5, 0.5, 0);
	m_plane->SetPoint1(pixmap.width(), 0, 0);
	m_plane->SetPoint2(0, pixmap.height(), 0);

	vtkSmartPointer<vtkTextureMapToPlane> map = vtkSmartPointer<vtkTextureMapToPlane>::New();
	map->SetInputConnection(m_plane->GetOutputPort());
	map->AutomaticPlaneGenerationOn();

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_mapper->SetInputConnection(map->GetOutputPort());
	initializePosition();
}

void BackgroundImageInfo::initializePosition()
{
	if (m_hasWorldFile) {
		readWorldFile(m_worldFilename);
	} else {
		fitImageToData();
	}
}

void BackgroundImageInfo::informChange()
{
	emit isChanged();
}

void BackgroundImageInfo::readWorldFile(const QString& name)
{
	double deltaX, deltaY;
	double leftTopX, leftTopY;
	double dummy1, dummy2;
	QFile file(name);
	file.open(QIODevice::ReadOnly);
	QTextStream stream(&file);

	stream >> deltaX;
	stream >> dummy1;
	stream >> dummy2;
	stream >> deltaY;
	stream >> leftTopX;
	stream >> leftTopY;

	file.close();

	m_scale = deltaX / m_resizeScale;
	m_translateX += leftTopX;
	m_translateY += leftTopY + (m_imageHeight)* deltaY;
	m_aspectRatio = m_imageHeight / static_cast<double>(m_imageWidth);

	if (fabs(deltaX) != fabs(deltaY)) {
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("In file %1, x-coordinate increment per pixel and y-coordinate increment per pixel mismatches."
												 " It is acceptable, but it may be wrong.").arg(QDir::toNativeSeparators(name)));
	}
}

void BackgroundImageInfo::fitImageToData()
{
	// fit to the data in the preprocessor window.
	double bounds[6];
	int vis = 0;
	if (m_preProcessorActor != nullptr) {
		vis = m_preProcessorActor->GetVisibility();
		m_preProcessorActor->SetVisibility(0);
	}
	iricMainWindow()->preProcessorWindow()->dataModel()->graphicsView()->mainRenderer()->ComputeVisiblePropBounds(bounds);
	if (m_preProcessorActor != nullptr) {
		m_preProcessorActor->SetVisibility(vis);
	}
	double targetX = bounds[1] - bounds[0];
	double targetY = bounds[3] - bounds[2];
	double targetAspectRatio = targetY / targetX;
	if (targetX < 0 || targetY < 0) { return; }

	m_translateX = bounds[0];
	m_translateY = bounds[2];

	double actorX = m_imageWidth;
	double actorY = m_imageHeight;
	double actorAspectRatio = m_imageHeight / static_cast<double>(m_imageWidth);
	m_aspectRatio = actorAspectRatio;

	double scale;
	if (targetAspectRatio > actorAspectRatio) {
		scale = targetX / actorX;
	} else {
		scale = targetY / actorY;
	}

	m_scale = scale / m_resizeScale;
	m_angle = 0;
}

void BackgroundImageInfo::mousePressEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_fixed) {return;}
	switch (event->button()) {
	case Qt::LeftButton:
		m_isTranslating = true;
		v->setCursor(m_moveCursor);
		break;
	case Qt::MidButton:
		m_isZooming = true;
		v->setCursor(m_zoomCursor);
		break;
	case Qt::RightButton:
		m_isRotating = true;
		v->setCursor(m_rotateCursor);
		break;
	default:
		// do nothing
		break;
	}
	m_isMoving = true;

	double pos[3];
	double scale[3];
	double ori[3];
	actor->GetPosition(pos);
	actor->GetScale(scale);
	actor->GetOrientation(ori);

	m_oldTranslateX = pos[0];
	m_oldTranslateY = pos[1];
	m_oldScale = scale[0];
	m_oldTheta = ori[2];

	lastX = event->x();
	lastY = event->y();
	dynamic_cast<VTK2DGraphicsView*>(v)->viewportToWorld(lastX, lastY);
}

void BackgroundImageInfo::mouseMoveEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_fixed) {return;}
	if (!(m_isRotating || m_isZooming || m_isTranslating)) { return; }
	double worldX = event->x();
	double worldY = event->y();
	VTK2DGraphicsView* view = dynamic_cast<VTK2DGraphicsView*>(v);
	view->viewportToWorld(worldX, worldY);

	if (m_isRotating) {
		double p[2];
		double q[2];
		p[0] = lastX - m_translateX;
		p[1] = lastY - m_translateY;
		q[0] = worldX - m_translateX;
		q[1] = worldY - m_translateY;

		double theta = p[0] * q[1] - p[1] * q[0];
		theta /= sqrt(p[0] * p[0] + p[1] * p[1]) * sqrt(q[0] * q[0] + q[1] * q[1]);
		theta = asin(theta) * 180 / M_PI;
		// rotate theta degrees about z axis.
		m_angle += theta;
	} else if (m_isZooming) {
		view->worldToViewport(lastX, lastY);
		double scaleFactor = 1 + (lastY - event->y()) / 100;
		m_scale *= scaleFactor;
	} else if (m_isTranslating) {
		m_translateX += (worldX - lastX);
		m_translateY += (worldY - lastY);
	}
	applySettingToActor(actor);
	lastX = worldX;
	lastY = worldY;
	view->GetRenderWindow()->Render();
}

void BackgroundImageInfo::mouseReleaseEvent(vtkActor* /*actor*/, QMouseEvent* /*event*/, VTKGraphicsView* v)
{
	if (m_fixed) {return;}
	if (! m_isMoving) {return;}
	m_isRotating = false;
	m_isZooming = false;
	m_isTranslating = false;

	iRICUndoStack::instance().push(new SetActorPropertyCommand(m_translateX, m_translateY, m_scale, m_angle, this));
	m_isMoving = false;
	v->unsetCursor();
}

void BackgroundImageInfo::toggleFixState()
{
	m_fixed = ! m_fixed;
	m_fixAction->setChecked(m_fixed);
	updateFixActionIcon();
}

void BackgroundImageInfo::updateFixActionIcon()
{
	QIcon icon;
	if (m_fixed) {
		icon = QIcon(":/libs/guicore/images/iconPinFixed.png");
	} else {
		icon = QIcon(":/libs/guicore/images/iconPinFree.png");
	}
	m_fixActionWithIcon->setIcon(icon);
}

void BackgroundImageInfo::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();

	m_filename   = elem.attribute("filename");
	m_angle      = iRIC::getDoubleAttribute(node, "angle", 0);
	m_translateX = iRIC::getDoubleAttribute(node, "positionX", 0);
	m_translateY = iRIC::getDoubleAttribute(node, "positionY", 0);
	m_scale      = iRIC::getDoubleAttribute(node, "scale", 1);
	m_visible    = iRIC::getBooleanAttribute(node, "visibility", true);
	m_fixed      = iRIC::getBooleanAttribute(node, "fixed", false);

	auto offset = this->offset();
	m_translateX -= offset.x();
	m_translateY -= offset.y();

	m_scale /= m_resizeScale;

	m_oldScale = m_scale;
	m_oldTranslateX = m_translateX;
	m_oldTranslateY = m_translateY;
	m_oldTheta = m_angle;
}

void BackgroundImageInfo::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	auto offset = this->offset();
	double translateX = m_translateX + offset.x();
	double translateY = m_translateY + offset.y();
	writer.writeAttribute("filename", m_filename);
	iRIC::setDoubleAttribute(writer, "angle", m_angle);
	iRIC::setDoubleAttribute(writer, "positionX", translateX);
	iRIC::setDoubleAttribute(writer, "positionY", translateY);
	iRIC::setDoubleAttribute(writer, "scale", m_scale * m_resizeScale);
	iRIC::setBooleanAttribute(writer, "visibility", m_visible);
	iRIC::setBooleanAttribute(writer, "fixed", m_fixed);
}

void BackgroundImageInfo::setupActor(vtkActor* actor)
{
	actor->SetMapper(m_mapper);
	actor->SetTexture(m_texture);
	applySettingToActor(actor);
}

void BackgroundImageInfo::applySettingToActor(vtkActor* actor)
{
	double pos[3];
	actor->GetPosition(pos);
	pos[0] = m_translateX;
	pos[1] = m_translateY;
	actor->SetPosition(pos);
	actor->SetScale(m_scale, m_scale, 1);
	actor->SetOrientation(0, 0, m_angle);

//	actor->SetVisibility(m_visible);
}

vtkActor* BackgroundImageInfo::refActor()
{
	applySettingToActor(m_refActor);
	return m_refActor;
}

QString BackgroundImageInfo::getThumbnailFileName(const QString& origname)
{
	QFileInfo finfo(origname);
	return QString("%1/%2_thumb.jpg").arg(finfo.absolutePath()).arg(finfo.baseName());
}

QDialog* BackgroundImageInfo::propertyDialog()
{
	BackgroundImageInfoDialog* dialog = new BackgroundImageInfoDialog();
	dialog->setInfo(this);
	dialog->setImageWidth(m_imageWidth);
	dialog->setProperties(m_translateX, m_translateY, m_scale, m_angle);
	return dialog;
}

void BackgroundImageInfo::handlePropertyDialogAccepted(QDialog* d)
{
	BackgroundImageInfoDialog* dialog = dynamic_cast<BackgroundImageInfoDialog*>(d);
	m_oldTheta = dialog->origAngle();
	m_oldScale = dialog->origScale();
	m_oldTranslateX = dialog->origLeftBottomX();
	m_oldTranslateY = dialog->origLeftBottomY();

	iRICUndoStack::instance().push(new SetActorPropertyCommand(dialog->leftBottomX(), dialog->leftBottomY(), dialog->scale(), dialog->angle(), this));
}

void BackgroundImageInfo::applyOffset(double x_diff, double y_diff)
{
	m_translateX -= x_diff;
	m_translateY -= y_diff;
}
