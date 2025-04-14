#include "geodatariversurvey.h"
#include "geodatariversurveyodnimporter.h"
#include "private/geodatariversurveyodnimporter_positionsettingdialog.h"

#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevation.h>

#include <vector>

GeoDataRiverSurveyOdnImporter::GeoDataRiverSurveyOdnImporter(GeoDataCreator* creator) :
	GeoDataImporter {"riverurvey_odn", tr("Cross-Section data (*.odn)"), creator}
{}

const QStringList GeoDataRiverSurveyOdnImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Cross-Section Data (*.odn)");
	return ret;
}
const QStringList GeoDataRiverSurveyOdnImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "odn";
	return ret;
}

bool GeoDataRiverSurveyOdnImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	auto rs = dynamic_cast<GeoDataRiverSurvey*> (data);
	rs->setEditMode();
	auto wse = rs->defaultWSE();

	QFile f(filename());
	if (! f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(w, GeoDataRiverSurveyOdnImporter::tr("Error"), GeoDataRiverSurveyOdnImporter::tr("Error occured while opening %1").arg(QDir::toNativeSeparators(filename())));
		return false;
	}

	QTextStream stream(&f);
	int lineNum = 1;
	std::vector<GeoDataRiverPathPoint*> points;
	std::vector<double> totalDistances;
	bool distanceContainsZero = false;
	double totalDistance = 0;
	while (true) {
		if (stream.atEnd()) {break;}

		QString str = stream.readLine();
		if (str.length() < 65) {
			QMessageBox::critical(w, GeoDataRiverSurveyOdnImporter::tr("Error"), GeoDataRiverSurveyOdnImporter::tr("Error occered while reading line %1. Too short.").arg(lineNum));
			return false;
		}
		auto name = QStringRef(&str, 15, 10).toString().trimmed();
		double x = 0;
		double y = - name.toFloat() * 1000;
		auto newPoint = new GeoDataRiverPathPoint(name, x, y, rs);
		newPoint->odn().setNb(2, QStringRef(&str, 0, 5).toInt() - 1);
		newPoint->odn().setNb(3, QStringRef(&str, 5, 5).toInt() - 1);
		double distance = QStringRef(&str, 25, 10).toDouble();
		newPoint->odn().setSpanDistance(distance);
		wse->addItem(name, true, QStringRef(&str, 35, 10).toDouble());
		newPoint->odn().setNb(1, QStringRef(&str, 45, 5).toInt() - 1);
		newPoint->odn().setNb(4, QStringRef(&str, 50, 5).toInt() - 1);
		newPoint->odn().setNb(0, QStringRef(&str, 55, 5).toInt() - 1);
		newPoint->odn().setNb(5, QStringRef(&str, 60, 5).toInt() - 1);

		auto pointCount = QStringRef(&str, 10, 5).toInt();
		int pointReadCount = 0;

		auto& cs = newPoint->crosssection();
		while (pointReadCount < pointCount) {
			str = stream.readLine();
			auto frags = str.split(" ", Qt::SkipEmptyParts);
			for (int i = 0; i < frags.size() / 2; ++i) {
				double pos = frags.at(i * 2).toDouble();
				double e = frags.at(i * 2 + 1).toDouble();

				cs.addPoint(pos, e);
			}
			pointReadCount += frags.size() / 2;
		}

		// shift
		double left = cs.leftBank().position();
		double right = cs.rightBank().position();
		double shiftValue = (left + right) * 0.5;

		newPoint->InhibitInterpolatorUpdate = true;
		newPoint->setCrosssectionDirection(QPointF(1, 0));
		newPoint->setPosition(QPointF(- shiftValue, y));
		newPoint->shiftCenter(shiftValue);
		newPoint->InhibitInterpolatorUpdate = false;

		points.push_back(newPoint);

		totalDistances.push_back(totalDistance);
		totalDistance += distance;

		++ lineNum;
	}
	auto tail = rs->headPoint();
	for (auto it = points.rbegin(); it != points.rend(); ++it) {
		auto p = *it;
		if (it != points.rbegin() && p->odn().spanDistance() == 0) {
			distanceContainsZero = true;
		}

		tail->addPathPoint(p);
		tail = p;
	}
	PositionSettingDialog dialog(w);
	if (distanceContainsZero) {
		dialog.disableDistance();
	}
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return false;
	}
	auto pos = dialog.position();
	if (pos == PositionSettingDialog::Position::Distance) {
		for (int i = 0; i < points.size(); ++i) {
			auto p = points[i];
			auto totalDistance = totalDistances[i];
			p->setPosition(QPointF(0, - totalDistance));
		}
	}

	rs->updateInterpolators();

	return true;
}
