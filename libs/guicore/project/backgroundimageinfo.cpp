#define _USE_MATH_DEFINES

#include "backgroundimageinfo.h"
//#include "main/iricmainwindow.h"
#include "../datamodel/graphicswindowdataitem.h"
//#include "project/projectmainfile.h"
#include "backgroundimageinfodialog.h"
#include "backgroundimageinfogeoreferencedialog.h"
#include "addiblegcptablemodel.h"
#include "gcptablerow.h"
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
#include <vtkProperty.h>
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

BackgroundImageInfo::BackgroundImageInfo(const QString& filename, const QString& origFilename, ProjectDataItem* d) :
	ProjectDataItem(d),
	m_name {filename},
	m_isRotating {false},
	m_isZooming {false},
	m_isTranslating {false},
	m_isGeoReferencing {false},
	m_isGeoReferenceSelectingPoint {false},
	m_geoReferenceActor {nullptr},
	m_geoReferenceParentWindow {nullptr},
	m_hasWorldFile {false},
	m_scale {1},
	m_angle {0},
	m_visible {true},
	m_imageWidth {0},
	m_imageHeight {0},
	m_resizeScale {1},
	m_fixed {false},
	m_movePixmap {":/libs/guicore/images/cursorImageMove.png"},
	m_rotatePixmap {":/libs/guicore/images/cursorImageRotate.png"},
	m_zoomPixmap {":/libs/guicore/images/cursorImageZoom.png"},
	m_moveCursor {m_movePixmap},
	m_rotateCursor {m_rotatePixmap},
	m_zoomCursor {m_zoomPixmap},
	m_georeferenceDialog {nullptr}
{
	QFileInfo finfo(filename);
	m_filename = finfo.fileName();
	m_caption = finfo.fileName();

	m_translateX = -offset().x();
	m_translateY = -offset().y();
	m_hide = false; // for georeference

	m_fixAction = new QAction(tr("Fix Image position"), this);
	m_fixAction->setCheckable(true);
	m_fixActionWithIcon = new QAction(tr("Fix Image position"), this);
	connect(m_fixAction, SIGNAL(triggered()), this, SLOT(toggleFixState()));
	connect(m_fixActionWithIcon, SIGNAL(triggered()), this, SLOT(toggleFixState()));
	updateFixActionIcon();
	readImageData(filename, origFilename);

	m_refActor = vtkSmartPointer<vtkActor>::New();
	setupActor(m_refActor);

	m_geoReferencePointsActor.pointsActor()->GetProperty()->SetPointSize(5);
	m_geoReferencePointsActor.pointsActor()->GetProperty()->SetColor(0, 0, 255);

	m_geoReferenceSelectedPointsActor.pointsActor()->GetProperty()->SetPointSize(7);
	m_geoReferenceSelectedPointsActor.pointsActor()->GetProperty()->SetColor(0, 0, 0);
}

BackgroundImageInfo::~BackgroundImageInfo()
{
}

const QString& BackgroundImageInfo::name() const
{
	return m_name;
}

void BackgroundImageInfo::setName(const QString& name)
{
	m_name = name;
}

const QString& BackgroundImageInfo::caption() const
{
	return m_caption;
}

void BackgroundImageInfo::setCaption(const QString& cap)
{
	m_caption = cap;
}

const QString& BackgroundImageInfo::fileName() const
{
	return m_filename;
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
	show();
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
	if (m_isGeoReferencing) {return;}

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
	if (m_isGeoReferencing) {return;}
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

void BackgroundImageInfo::mouseReleaseEvent(vtkActor* /*actor*/, QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_fixed) {return;}
	if (m_isGeoReferencing && m_isGeoReferenceSelectingPoint) {
		if (event->button() != Qt::LeftButton) {return;}

		double worldX = event->x();
		double worldY = event->y();
		VTK2DGraphicsView* view = dynamic_cast<VTK2DGraphicsView*>(v);
		view->viewportToWorld(worldX, worldY);
		emit gcpDefined(QPointF(worldX, worldY));

		std::unordered_set<std::vector<GcpTableRow>::size_type> indices;
		updateGeoReferencePointsActor(indices);
		return;
	}
	m_isRotating = false;
	m_isZooming = false;
	m_isTranslating = false;

	iRICUndoStack::instance().push(new SetActorPropertyCommand(m_translateX, m_translateY, m_scale, m_angle, this));
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

bool BackgroundImageInfo::visible() const
{
	return m_visible;
}

void BackgroundImageInfo::setVisible(bool visible)
{
	m_visible = visible;
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

double BackgroundImageInfo::translateX() const
{
	return m_translateX;
}

double BackgroundImageInfo::translateY() const
{
	return m_translateY;
}

double BackgroundImageInfo::scale() const
{
	return m_scale;
}

double BackgroundImageInfo::resizeScale() const
{
	return m_resizeScale;
}

double BackgroundImageInfo::angle() const
{
	return m_angle;
}

void BackgroundImageInfo::setPreProcessorActor(vtkActor* actor)
{
	m_preProcessorActor = actor;
}

void BackgroundImageInfo::setTranslateX(double x)
{
	m_translateX = x;
}

void BackgroundImageInfo::setTranslateY(double y)
{
	m_translateY = y;
}

void BackgroundImageInfo::setScale(double scale)
{
	m_scale = scale;
}

vtkActor* BackgroundImageInfo::refActor()
{
	applySettingToActor(m_refActor);
	return m_refActor;
}

QAction* BackgroundImageInfo::fixAction() const
{
	return m_fixAction;
}

QAction* BackgroundImageInfo::fixActionWithIcon() const
{
	return m_fixActionWithIcon;
}

double BackgroundImageInfo::aspectRatio() const
{
	return m_aspectRatio;
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

QDialog* BackgroundImageInfo::georeferenceDialog(QWidget* w)
{
	m_geoReferenceParentWindow = w;
	return new BackgroundImageInfoGeoreferenceDialog(this, w);
}

void BackgroundImageInfo::updateGeoReferencePointsActor(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indice)
{
	auto table = gcpTable();
	std::vector<QPointF> points;
	for (GcpTableRow row : *table) {
		QPointF p(row.destX, row.destY);
		points.push_back(p);
	}
	m_geoReferencePointsActor.setLine(points);

	std::vector<QPointF> selectedPoints;
	for (auto idx : indice) {
		if (! (idx < table->size())) {continue;}
		auto row = table->at(idx);
		QPointF p(row.destX, row.destY);
		selectedPoints.push_back(p);
	}
	m_geoReferenceSelectedPointsActor.setLine(selectedPoints);

	m_geoReferenceGraphicsView->render();
}

std::vector<GcpTableRow>* BackgroundImageInfo::gcpTable()
{
	return dynamic_cast<BackgroundImageInfoGeoreferenceDialog*> (m_georeferenceDialog)->gcpTable();
}

GcpTableModel* BackgroundImageInfo::gcpTableModel()
{
	return dynamic_cast<BackgroundImageInfoGeoreferenceDialog*> (m_georeferenceDialog)->gcpTableModel();
}

void BackgroundImageInfo::handleGeoreferenceDialogAccepted(QDialog* d)
{
	BackgroundImageInfoGeoreferenceDialog* dialog = dynamic_cast<BackgroundImageInfoGeoreferenceDialog*> (d);
	m_oldTheta = dialog->origAngle();
	m_oldScale = dialog->origScale();
	m_oldTranslateX = dialog->origLeftBottomX();
	m_oldTranslateY = dialog->origLeftBottomY();

	iRICUndoStack::instance().push(new SetActorPropertyCommand(dialog->leftBottomX(), dialog->leftBottomY(), dialog->scale(), dialog->angle(), this));
}

void BackgroundImageInfo::selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices)
{
	updateGeoReferencePointsActor(indices);
}

void BackgroundImageInfo::startGcpSelect()
{
	QMessageBox::information(m_geoReferenceParentWindow, tr("Information"), tr("Select a corresponding point on Main Window."));
	m_isGeoReferenceSelectingPoint = true;
}

void BackgroundImageInfo::showGeoreferenceDialog(vtkActor* actor, VTKGraphicsView* v, double minDepth, double maxDepth, QWidget* w)
{
	if (m_georeferenceDialog == nullptr) {
		m_georeferenceDialog = georeferenceDialog(w);

		auto f = [&]() {handleGeoreferenceDialogAccepted(m_georeferenceDialog);};
		connect(m_georeferenceDialog, &BackgroundImageInfoGeoreferenceDialog::accepted, this, f);
		connect(m_georeferenceDialog, SIGNAL(destroyed()), this, SLOT(handleGeoreferenceDialogClosed()));
	}
	m_geoReferenceActor = actor;
	m_geoReferenceGraphicsView = v;
	m_geoReferencePointsActor.pointsActor()->SetPosition(0, 0, maxDepth);
	m_geoReferenceGraphicsView->mainRenderer()->AddActor(m_geoReferencePointsActor.pointsActor());
	m_geoReferenceSelectedPointsActor.pointsActor()->SetPosition(0, 0, (minDepth + maxDepth) * 0.5);
	m_geoReferenceGraphicsView->mainRenderer()->AddActor(m_geoReferenceSelectedPointsActor.pointsActor());

	std::unordered_set<std::vector<GcpTableRow>::size_type> indices;
	updateGeoReferencePointsActor(indices);

	m_isGeoReferencing = true;
	m_isGeoReferenceSelectingPoint = false;

	m_georeferenceDialog->show();
	m_georeferenceDialog->raise();
	m_georeferenceDialog->activateWindow();
}

void BackgroundImageInfo::handleGeoreferenceDialogClosed()
{
	m_isGeoReferencing = false;
	m_geoReferenceGraphicsView->mainRenderer()->RemoveActor(m_geoReferencePointsActor.pointsActor());
	m_geoReferenceGraphicsView->mainRenderer()->RemoveActor(m_geoReferenceSelectedPointsActor.pointsActor());

	m_geoReferenceActor = nullptr;
	m_georeferenceDialog = nullptr;
	show();

	emit isGeoreferenceDialogClosed();
}

void BackgroundImageInfo::hide()
{
	if (! m_isGeoReferencing) {return;}

	m_hide = true;
	m_geoReferenceActor->VisibilityOff();
	m_geoReferenceGraphicsView->render();
	emit isVisibilityChanged();
}

void BackgroundImageInfo::show()
{
	if (! m_isGeoReferencing) {return;}

	m_hide = false;
	m_geoReferenceActor->VisibilityOn();
	m_geoReferenceGraphicsView->render();
	emit isVisibilityChanged();
}

void BackgroundImageInfo::toggleVisibility()
{
	if (! m_isGeoReferencing) {return;}

	m_hide = ! m_hide;
	int vis = 1;
	if (m_hide) {vis = 0;}
	m_geoReferenceActor->SetVisibility(vis);
	m_geoReferenceGraphicsView->render();
	emit isVisibilityChanged();
}

bool BackgroundImageInfo::isVisible()
{
	return ! m_hide;
}
