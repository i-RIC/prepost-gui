#include "geodatapointmapt.h"
#include "geodatapointmapwebimporter.h"
#include "geodatapointmapwebimporterregionselectdialog.h"
#include "geodatapointmapwebimporterzoomleveldialog.h"

#include <cs/coordinatesystem.h>
#include <cs/webmercatorutil.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/informationdialog.h>
#include <misc/networksetting.h>

#include <QApplication>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QThread>

#include <vtkDoubleArray.h>

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
			QString urlstr = m_url;
			urlstr.replace("{z}", QString::number(m_zoomLevel));
			urlstr.replace("{x}", QString::number(x));
			urlstr.replace("{y}", QString::number(y));

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

			if (m_webReply->error() == QNetworkReply::OperationCanceledError) {
				goto FINISH;
			}

			if (m_webReply->error() != QNetworkReply::NoError) {
				InformationDialog::critical(w, tr("Error"), m_webReply->errorString(), QString("WebReplyError_%1").arg(m_webReply->error()));
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
					bool ok;
					double v = valstr.toDouble(&ok);
					if (! ok) {continue;}

					double lon, lat;
					m_wmUtil->getCoordinates(x, y, col, row, &lon, &lat);

					if (lon < m_lonMin || lon > m_lonMax) {continue;}
					if (lat < m_latMin || lat > m_latMax) {continue;}

					double cx, cy;
					if (! m_coordinateSystem->isLongLat()) {
						m_coordinateSystem->mapGeoToGrid(lon, lat, &cx, &cy);
					} else {
						cx = lon;
						cy = lat;
					}

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

	if (points->GetNumberOfPoints() == 0) {return false;}

	pmap->setPoints(points, values);
	if (points->GetNumberOfPoints() > 3) {
		pmap->doDelaunay();
	}
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
		int dialogRet = mainfile->showCoordinateSystemDialog(true);
		if (dialogRet == QDialog::Rejected) {
			return false;
		}
	}
	m_coordinateSystem = mainfile->coordinateSystem();
	m_coordinateSystem->init();

	GeoDataPointmapWebImporterRegionSelectDialog rsDialog(w);
	rsDialog.setCoordinateSystem(m_coordinateSystem);

	int ret = rsDialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	m_lonMin = rsDialog.minLon();
	m_lonMax = rsDialog.maxLon();
	m_latMin = rsDialog.minLat();
	m_latMax = rsDialog.maxLat();

	GeoDataPointmapWebImporterZoomLevelDialog zlDialog(w);
	zlDialog.setArea(m_lonMin, m_lonMax, m_latMin, m_latMax);

	ret = zlDialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	m_zoomLevel = zlDialog.zoomLevel();
	m_url = zlDialog.url();

	m_wmUtil = new WebMercatorUtil(m_zoomLevel);

	return true;
}
