#include "googlemapimageimporter.h"
#include "googlemapimageimportsettingdialog.h"

#include <cs/coordinatesystem.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/errormessage.h>
#include <misc/networksetting.h>
#include <tmsloader/tmsutil.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QSettings>
#include <QThread>

#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

const int GoogleMapImageImporter::MAX_GOOGLEAPI_COUNT = 50;

namespace {

int imageCount(const QSize& size)
{
	int iCount = static_cast<int>(std::ceil(size.width() / 640));
	int jCount = static_cast<int>(std::ceil(size.height() / 640));

	return iCount * jCount;
}

} // namespace

GoogleMapImageImporter::GoogleMapImageImporter(ProjectData* projectData, iRICMainWindowInterface* parent) :
	QObject(parent)
{
	m_projectData = projectData;
	m_googleMapAccessManager = new QNetworkAccessManager(this);
	m_googleMapFile = nullptr;
	m_isWaitingHttpResponse = false;

	NetworkSetting setting;
	QList<QNetworkProxy> proxies = setting.queryProxy();
	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::NoProxy);
	if (proxies.size() != 0) {
		proxy = proxies.first();
	}
	m_googleMapAccessManager->setProxy(proxy);
}

void GoogleMapImageImporter::importImages()
{
	ProjectMainFile* mainFile = m_projectData->mainfile();
	CoordinateSystem* coordSystem = mainFile->coordinateSystem();
	if (coordSystem == nullptr) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("To import background image using Google Map API, you must specify coordinate system for this project."));
		return;
	}
	double xmin, xmax, ymin, ymax;
	mainWindow()->preProcessorWindow()->dataModel()->graphicsView()->getDataRegion(&xmin, &xmax, &ymin, &ymax);
	double offsetX = mainFile->offset().x();
	double offsetY = mainFile->offset().y();
	xmin += offsetX;
	xmax += offsetX;
	ymin += offsetY;
	ymax += offsetY;

	double xcenter = (xmin + xmax) * 0.5;
	double ycenter = (ymin + ymax) * 0.5;

	double centerLon, centerLat;
	double width = qMax(xmax - xmin, ymax - ymin);
	QSizeF regionSize(xmax - xmin, ymax - ymin);
	if (width < 1.0E-6) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("This function can not be used when there is no data to be drawn."));
		return;
	}
	coordSystem->mapGridToGeo(xcenter, ycenter, &centerLon, &centerLat);

	// calculate meterPerPixel and zoomLevel for 1280 pixel width image
	double mpp = width / 1280;
	QPointF centerLonLat(centerLon, centerLat);
	int zoomLevel = tmsloader::TmsUtil::calcNativeZoomLevel(centerLonLat, mpp);
	if (zoomLevel > 20) {zoomLevel = 20;}

	QSize imgSize = tmsloader::TmsUtil::calcPixelSize(centerLonLat, regionSize, zoomLevel);

	int maxZoomLevel = zoomLevel;
	while (imageCount(imgSize) < MAX_GOOGLEAPI_COUNT) {
		++ maxZoomLevel;
		imgSize *= 2;
	}
	-- maxZoomLevel;

	if (maxZoomLevel > 20) {maxZoomLevel = 20;}

	GoogleMapImageImportSettingDialog dialog(mainWindow());
	dialog.setZoom(zoomLevel);
	dialog.setZoomMax(maxZoomLevel);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	int zoom = dialog.zoom();
	QString maptype = dialog.maptype();
	mpp = tmsloader::TmsUtil::meterPerPixel(centerLonLat, zoom);

	// copy to the project file.
	if (! mainFile->mkdirBGDIR()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("The background image was not added. Please try again."));
		return;
	}
	imgSize = tmsloader::TmsUtil::calcPixelSize(centerLonLat, regionSize, zoom);
	int icount = static_cast<int>(std::floor(imgSize.width() / 640)) + 1;
	int jcount = static_cast<int>(std::floor(imgSize.height() / 640)) + 1;

	WaitDialog* wdialog = new WaitDialog(mainWindow());
	connect(wdialog, SIGNAL(canceled()), this, SLOT(abortGoogleMapRequest()));
	wdialog->show();

	for (int j = 0; j < jcount; ++j) {
		for (int i = 0; i < icount; ++i) {
			double tmpCenterX = xmin + regionSize.width() * (0.5 + i) / icount;
			double tmpCenterY = ymin + regionSize.height() * (0.5 + j) / jcount;

			coordSystem->mapGridToGeo(tmpCenterX, tmpCenterY, &centerLon, &centerLat);
			QString centerLonStr = QString::number(centerLon, 'g', 12);
			QString centerLatStr = QString::number(centerLat, 'g', 12);

			QString urlstr = QString("http://maps.googleapis.com/maps/api/staticmap?center=%1,%2&zoom=%3&size=640x640&sensor=false&maptype=%4").arg(centerLatStr).arg(centerLonStr).arg(zoom).arg(maptype);
			m_isWaitingHttpResponse = true;

			QUrl url(urlstr);
			QNetworkRequest request(url);

			m_googleMapReply = m_googleMapAccessManager->get(request);
			connect(m_googleMapReply, SIGNAL(finished()), this, SLOT(handleGoogleMapDone()));

			qApp->processEvents();
			while (m_isWaitingHttpResponse) {
				qApp->thread()->wait(100);
				qApp->processEvents();
			}

			if (m_googleMapReply->error() != QNetworkReply::NoError) {
				if (m_googleMapReply->error() != QNetworkReply::OperationCanceledError) {
					QMessageBox::critical(mainWindow(), tr("Error"), m_googleMapReply->errorString());
				}
				goto FINISH_GOOGLEAPI;
			}
			try {
				QString fname = getNextGoogleMapFileName();
				QFile f(fname);
				bool ok = f.open(QIODevice::WriteOnly);
				if (! ok) {
					QMessageBox::warning(mainWindow(), tr("Warning"), tr("The background image file could not be opened. Please try again."));
					f.close();
					goto FINISH_GOOGLEAPI;
				}
				f.write(m_googleMapReply->readAll());
				f.close();
				BackgroundImageInfo* image = new BackgroundImageInfo(fname, fname, mainFile);

				double xleft = tmpCenterX - mpp * 320;
				double ybottom = tmpCenterY - mpp * 320;
				image->setTranslateX(xleft - offsetX);
				image->setTranslateY(ybottom - offsetY);
				image->setScale(mpp);

				mainFile->addBackgroundImage(image);
			} catch (ErrorMessage m) {
				QMessageBox::warning(mainWindow(), tr("Warning"), m);
			}
		}
	}

FINISH_GOOGLEAPI:
	m_googleMapReply = nullptr;

	wdialog->hide();
	delete wdialog;

}

void GoogleMapImageImporter::abortGoogleMapRequest()
{
	if (m_googleMapReply == nullptr) {return;}
	m_googleMapReply->abort();
}

void GoogleMapImageImporter::handleGoogleMapDone()
{
	m_isWaitingHttpResponse = false;
}

iRICMainWindowInterface* GoogleMapImageImporter::mainWindow()
{
	return dynamic_cast<iRICMainWindowInterface*>(parent());
}

QString GoogleMapImageImporter::getNextGoogleMapFileName()
{
	QString fullbgdir = m_projectData->workDirectory();
	fullbgdir.append("/").append(ProjectMainFile::BGDIR);
	QDir bgDir = QDir(fullbgdir);
	int fidx = 1;

	QString targetName;
	do {
		targetName = bgDir.absoluteFilePath(QString("GoogleMap%1.png").arg(fidx));
		++ fidx;
	} while (QFile(targetName).exists());
	return targetName;
}
