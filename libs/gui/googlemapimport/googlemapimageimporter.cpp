#include "../misc/networksetting.h"
#include "googlemapimageimporter.h"
#include "googlemapimageimportsettingdialog.h"

#include <guibase/coordinatesystem.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/backgroundimageinfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/errormessage.h>

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

// Meter per pixel at equator for zoom level 1 = 40075334.2563 / 512.;
const double GoogleMapImageImporter::METERPERPIXEL_AT_EQUATOR_1 = 78272.137219; //
const int GoogleMapImageImporter::MAX_GOOGLEAPI_COUNT = 50;

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
	double xcenter = (xmin + xmax) * 0.5;
	double ycenter = (ymin + ymax) * 0.5;

	double centerLon, centerLat;
	double width = qMax(xmax - xmin, ymax - ymin);
	if (width < 1.0E-6) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("This function can not be used when there is no data to be drawn."));
		return;
	}
	coordSystem->mapGridToGeo(xcenter, ycenter, &centerLon, &centerLat);

	int zoomlevel = 1;
	double meterPerPixel = METERPERPIXEL_AT_EQUATOR_1;
	meterPerPixel *= std::cos(centerLat / 180 * 3.141592);

	while (width / meterPerPixel < 640 && zoomlevel <= 20) {
		++ zoomlevel;
		meterPerPixel /= 2.;
	}

	int maxzoomlevel = zoomlevel;

	int gcount = 1;
	while (gcount * gcount <= MAX_GOOGLEAPI_COUNT) {
		++ gcount;
	}
	-- gcount;
	if (gcount == 0) {gcount = 1;}

	while (width / meterPerPixel < (640 * gcount) && maxzoomlevel <= 20) {
		++ maxzoomlevel;
		meterPerPixel /= 2.0;
	}
	-- maxzoomlevel;
	if (maxzoomlevel < zoomlevel) {maxzoomlevel = zoomlevel;}

	GoogleMapImageImportSettingDialog dialog(mainWindow());
	dialog.setZoom(zoomlevel);
	dialog.setZoomMax(maxzoomlevel);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	coordSystem->mapGeoToGrid(centerLon, centerLat, &xcenter, &ycenter);

	int zoom = dialog.zoom();
	QString maptype = dialog.maptype();

	zoomlevel = 1;
	// meterPerPixel at equator for zoom level 1
	meterPerPixel = METERPERPIXEL_AT_EQUATOR_1;
	meterPerPixel *= std::cos(centerLat / 180 * 3.141592);

	while (zoomlevel < zoom) {
		++ zoomlevel;
		meterPerPixel /= 2.;
	}

	// copy to the project file.
	if (! mainFile->mkdirBGDIR()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("The background image was not added. Please try again."));
		return;
	}
	int pixelWidth = static_cast<int>(width / meterPerPixel);
	int icount = ((pixelWidth - 1) / 640) + 1;
	if (icount > gcount) {icount = gcount;}

	WaitDialog* wdialog = new WaitDialog(mainWindow());
	connect(wdialog, SIGNAL(canceled()), this, SLOT(abortGoogleMapRequest()));
	wdialog->show();

	for (int j = 0; j < icount; ++j) {
		for (int i = 0; i < icount; ++i) {
			double tmpCenterX = xcenter + ((i + 0.5) / icount - 0.5) * width;
			double tmpCenterY = ycenter + ((j + 0.5) / icount - 0.5) * width;

			coordSystem->mapGridToGeo(tmpCenterX, tmpCenterY, &centerLon, &centerLat);
			QString urlstr = QString("http://maps.googleapis.com/maps/api/staticmap?center=%1,%2&zoom=%3&size=640x640&sensor=false&maptype=%4").arg(centerLat).arg(centerLon).arg(zoom).arg(maptype);
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

				double xleft = tmpCenterX - meterPerPixel * 320;
				double ybottom = tmpCenterY - meterPerPixel * 320;
				image->setTranslateX(xleft);
				image->setTranslateY(ybottom);
				image->setScale(meterPerPixel);

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
