#include "geodatapointmapt.h"
#include "geodatapointmapwebimporter.h"
#include "geodatapointmapwebimporterregiondialog.h"

#include <cs/coordinatesystem.h>
#include <cs/webmeratorutil.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/networksetting.h>

#include <QApplication>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QThread>

GeoDataPointmapWebImporter::GeoDataPointmapWebImporter(GeoDataCreator* creator) :
	GeoDataWebImporter("csvtile", tr("Web CSV tile"), creator),
	m_webAccessManager {new QNetworkAccessManager(this)},
	m_webReply {nullptr},
	m_isWaitingHttpResponse {false},
	m_coordinateSystem {nullptr},
	m_wmUtil {nullptr}
{
	NetworkSetting setting;
	QList<QNetworkProxy> proxies = setting.queryProxy();
	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::NoProxy);
	if (proxies.size() != 0) {
		proxy = proxies.first();
	}
	m_webAccessManager->setProxy(proxy);
}

GeoDataPointmapWebImporter::~GeoDataPointmapWebImporter()
{
	delete m_wmUtil;
}

bool GeoDataPointmapWebImporter::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (condition->name() == "Elevation") {return true;}

	return false;
}

bool GeoDataPointmapWebImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkDoubleArray> values = vtkSmartPointer<vtkDoubleArray>::New();

	int xmin, xmax, ymin, ymax;

	m_wmUtil->getTileRegion(m_lonMin, m_latMax, m_lonMax, m_latMin, &xmin, &xmax, &ymin, &ymax);

	WaitDialog* dialog = new WaitDialog(w);
	connect(dialog, SIGNAL(canceled()), this, SLOT(abortRequest()));
	int count = (ymax - ymin + 1) * (xmax - xmin + 1);

	dialog->showProgressBar();
	dialog->setRange(0, count);
	dialog->setProgress(0);
	dialog->show();

	int index = 0;
	for (int y = ymin; y <= ymax; ++y) {
		for (int x = xmin; x <= xmax; ++x) {
			QString urlstr = QString("http://cyberjapandata.gsi.go.jp/xyz/dem/%1/%2/%3.txt").arg(m_zoomLevel).arg(x).arg(y);
			QUrl url(urlstr);
			QNetworkRequest request(url);

			m_webReply = m_webAccessManager->get(request);
			connect(m_webReply, SIGNAL(finished()), this, SLOT(handleDone()));
			m_isWaitingHttpResponse = true;

			qApp->processEvents();
			while (m_isWaitingHttpResponse) {
				qApp->thread()->wait(100);
				qApp->processEvents();
			}

			if (m_webReply->error() != QNetworkReply::NoError) {
				if (m_webReply->error() != QNetworkReply::OperationCanceledError) {
					QMessageBox::critical(w, tr("Error"), m_webReply->errorString());
				}
				goto FINISH;
			}

			for (int row = 0; row < 256; ++row) {
				QByteArray buffer = m_webReply->readLine();
				QString csvLine(buffer);
				csvLine = csvLine.trimmed();
				if (csvLine.isEmpty()) {break;}

				QStringList pieces = csvLine.split(',');
				int cols = pieces.size();
				for (int col = 0; col < cols; ++col) {
					QString valstr = pieces[col];
					if (valstr == "e") { continue; }

					double v = valstr.toDouble();
					double lon, lat;
					m_wmUtil->getCoordinates(x, y, col, row, &lon, &lat);

					double cx, cy;
					m_coordinateSystem->mapGeoToGrid(lon, lat, &cx, &cy);

					points->InsertNextPoint(cx, cy, 0);
					values->InsertNextValue(v);
				}
			}
			dialog->setProgress(++ index);
		}
	}

FINISH:
	dialog->close();
	delete dialog;

	pmap->setPoints(points, values);
	pmap->doDelaunay();
	return true;
}

void GeoDataPointmapWebImporter::abortRequest()
{
	if (m_webReply == nullptr) {return;}
	m_webReply->abort();
}

void GeoDataPointmapWebImporter::handleDone()
{
	m_isWaitingHttpResponse = false;
}

bool GeoDataPointmapWebImporter::doInit(int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	*count = 1;
	auto mainfile = item->projectData()->mainfile();
	if (mainfile->coordinateSystem() == nullptr) {
		int dialogRet = mainfile->showCoordinateSystemDialog();
		if (dialogRet == QDialog::Rejected) {
			return false;
		}
	}
	m_coordinateSystem = mainfile->coordinateSystem();
	m_coordinateSystem->init();

	m_lonMin = 139.5;
	m_lonMax = 140.0;
	m_latMin = 35.5;
	m_latMax = 36.0;

	double centerLat = (m_latMin + m_latMax) * 0.5;

	GeoDataPointmapWebImporterRegionDialog dialog(w);
	dialog.setCenterLatitude(centerLat);
	dialog.setMaxZoomLevel(12);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	m_zoomLevel = dialog.zoomLevel();

	m_wmUtil = new WebMeratorUtil(m_zoomLevel);

	return true;
}
