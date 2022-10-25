#include "backgroundimageinfo_impl.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QIcon>

#include <vtkPlaneSource.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>

BackgroundImageInfo::Impl::Impl(BackgroundImageInfo* info) :
	m_originalImageWidth {0},
	m_originalImageHeight {0},
	m_resizeScale {1},
	m_texture {vtkTexture::New()},
	m_plane {vtkPlaneSource::New()},
	m_mapToPlane {vtkTextureMapToPlane::New()},
	m_preProcessorActor {nullptr},
	m_isGeoReferencing {false},
	m_isGeoReferenceSelectingPoint {false},
	m_geoReferenceActor {nullptr},
	m_geoReferencePointsActor {},
	m_geoReferenceSelectedPointsActor {},
	m_geoReferenceGraphicsView {nullptr},
	m_geoReferenceParentWindow {nullptr},
	m_fixAction {new QAction(BackgroundImageInfo::tr("Fix Image position"), info)},
	m_fixActionWithIcon {new QAction(BackgroundImageInfo::tr("Fix Image position"), info)},
	m_movePixmap {":/libs/guicore/images/cursorImageMove.png"},
	m_rotatePixmap {":/libs/guicore/images/cursorImageRotate.png"},
	m_zoomPixmap {":/libs/guicore/images/cursorImageZoom.png"},
	m_moveCursor {m_movePixmap},
	m_rotateCursor {m_rotatePixmap},
	m_zoomCursor {m_zoomPixmap},
	m_mouseEventMode {MouseEventMode::Normal},
	m_parent {info}
{
	m_fixAction->setCheckable(true);

	setupGeoReferenceActors();
}

BackgroundImageInfo::Impl::~Impl()
{
	m_texture->Delete();
	m_plane->Delete();
	m_mapToPlane->Delete();
}

void BackgroundImageInfo::Impl::setupGeoReferenceActors()
{
	m_geoReferencePointsActor.pointsActor()->GetProperty()->SetPointSize(5);
	m_geoReferencePointsActor.pointsActor()->GetProperty()->SetColor(0, 0, 255);

	m_geoReferenceSelectedPointsActor.pointsActor()->GetProperty()->SetPointSize(7);
	m_geoReferenceSelectedPointsActor.pointsActor()->GetProperty()->SetColor(0, 0, 0);
}

void BackgroundImageInfo::Impl::loadImageData(const QString& fileName)
{
	QFileInfo finfo(fileName);
	vtkSmartPointer<vtkImageReader2> reader;

	auto ext = finfo.suffix().toLower();

	if (ext == "jpg" || ext == "jpeg") {
		reader = vtkSmartPointer<vtkJPEGReader>::New();
	} else if (ext == "png") {
		reader = vtkSmartPointer<vtkPNGReader>::New();
	} else if (ext == "tif") {
		reader = vtkSmartPointer<vtkTIFFReader>::New();
	}

	QPixmap pixmap;
	bool ok = pixmap.load(fileName);
	if (! ok) {
		throw ErrorMessage(tr("Unable to read image %1").arg(finfo.fileName()));
	}

	bool scaled = false;
	m_resizeScale = 1;

	m_originalImageWidth = pixmap.width();
	m_originalImageHeight = pixmap.height();

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

	auto scaledImageName = scaledImageFileName(fileName);

	if (scaled) {
		// if scaled to smaller file, it is saved.
		pixmap.save(scaledImageName);
	} else {
		// thumbnail not needed. If it exists, remove it.
		QFile::remove(scaledImageName);
	}

	QFile scaledImage(scaledImageName);
	if (scaledImage.exists()) {
		reader = vtkSmartPointer<vtkJPEGReader>::New();
		reader->SetFileName(iRIC::toStr(scaledImageName).c_str());
	} else {
		reader->SetFileName(iRIC::toStr(fileName).c_str());
	}
	reader->Update();

	m_texture->SetInputData(reader->GetOutput());
	m_texture->InterpolateOn();
	m_plane->SetCenter(0.5, 0.5, 0);
	m_plane->SetPoint1(pixmap.width(), 0, 0);
	m_plane->SetPoint2(0, pixmap.height(), 0);

	m_mapToPlane->AutomaticPlaneGenerationOn();
	m_mapToPlane->SetInputConnection(m_plane->GetOutputPort());
}

void BackgroundImageInfo::Impl::updateFixActionIcon()
{
	QIcon icon;
	if (m_setting.fixed) {
		icon = QIcon(":/libs/guicore/images/iconPinFixed.svg");
	} else {
		icon = QIcon(":/libs/guicore/images/iconPinFree.svg");
	}
	m_fixActionWithIcon->setIcon(icon);
}

void BackgroundImageInfo::Impl::applyWorldFileSetting(const QString& fileName)
{
	double deltaX, deltaY;
	double leftTopX, leftTopY;
	double dummy1, dummy2;
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	QTextStream stream(&file);

	stream >> deltaX;
	stream >> dummy1;
	stream >> dummy2;
	stream >> deltaY;
	stream >> leftTopX;
	stream >> leftTopY;

	file.close();

	m_setting.scale = deltaX;
	m_setting.angle = 0;
	m_setting.positionX = leftTopX;
	m_setting.positionY = leftTopY + m_originalImageHeight * deltaY;
}

void BackgroundImageInfo::Impl::fitToRegion(double xmin, double xmax, double ymin, double ymax)
{
	if ((xmax < xmin || ymax < ymin)) {
		// no data
		auto& s = m_setting;
		s.positionX = 0;
		s.positionY = 0;
		s.angle = 0;
		s.scale = 1;

		return;
	}
	auto& s = m_setting;
	s.positionX = xmin;
	s.positionY = ymin;
	s.angle = 0;

	double dataAspectRatio = (ymax - ymin) / (xmax - xmin);
	double imageAspectRatio = m_originalImageHeight / static_cast<double> (m_originalImageWidth);

	if (dataAspectRatio > imageAspectRatio) {
		s.scale = (xmax - xmin) / m_originalImageWidth;
	} else {
		s.scale = (ymax - ymin) / m_originalImageHeight;
	}
}

QString BackgroundImageInfo::Impl::scaledImageFileName(const QString origName)
{
	QFileInfo finfo(origName);
	return QString("%1/%2_thumb.jpg").arg(finfo.absolutePath()).arg(finfo.baseName());
}
