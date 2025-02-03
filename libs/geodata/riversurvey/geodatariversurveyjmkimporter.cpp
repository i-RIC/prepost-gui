#include "geodatariversurvey.h"
#include "geodatariversurveyjmkimporter.h"
#include "geodatariverpathpoint.h"
#include "geodatariverpathpointjmkdata.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <unordered_map>

GeoDataRiverSurveyJmkImporter::GeoDataRiverSurveyJmkImporter() :
	QObject {}
{}

bool GeoDataRiverSurveyJmkImporter::import(const QString& filename, GeoDataRiverSurvey* rs, QWidget* w)
{
	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	std::unordered_map<QString, GeoDataRiverPathPoint*> points;
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != nullptr) {
		points.insert({p->name(), p});
		p = p->nextPoint();
	}

	QTextStream stream(&file);
	while (true) {
		if (stream.atEnd()) {break;}

		QString str = stream.readLine();
		if (str[0] == 'E') {break;}

		int num = QStringRef(&str, 0, 5).toInt();
		QString name = QStringRef(&str, 5, 8).toString().trimmed();
		int blocks = QStringRef(&str, 13, 5).toInt();

		auto it = points.find(name);
		if (it == points.end()) {
			// skip
			for (int i = 0; i < blocks; ++i) {
				str = stream.readLine();
			}
		} else {
			auto point = it->second;
			point->jmk().items().clear();

			for (int i = 0; i < blocks; ++i) {
				str = stream.readLine();
				GeoDataRiverPathPointJmkData::Item item;
				item.distance = QStringRef(&str, 0, 8).toDouble();
				item.width = QStringRef(&str, 8, 8).toDouble();
				item.height = QStringRef(&str, 16, 8).toDouble();
				item.submerged = QStringRef(&str, 24, 5).toInt();
				item.dense = QStringRef(&str, 29, 5).toInt();
				item.dead = QStringRef(&str, 34, 5).toInt();
				item.highLow = QStringRef(&str, 39, 5).toInt();
				item.lowBranchHeight = QStringRef(&str, 44, 8).toDouble();

				point->jmk().items().push_back(item);
			}
		}
	}
	file.close();

	return true;
}
