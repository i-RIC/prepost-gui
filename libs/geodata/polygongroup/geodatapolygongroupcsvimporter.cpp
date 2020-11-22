#include "geodatapolygongroup.h"
#include "geodatapolygongroupcsvimporter.h"
#include "geodatapolygongrouppolygon.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPolygonF>
#include <QTextStream>

namespace {

void addNewPolygon(GeoDataPolygonGroup* group, const QString& name, double value, QPolygonF polygon)
{
	if (polygon.at(0) != polygon.at(polygon.size() - 1)) {
		polygon.append(polygon.at(0));
	}

	std::vector<QPolygonF> holes;
	auto newLine = new GeoDataPolygonGroupPolygon(polygon, holes, group);
	newLine->setName(name);
	newLine->setValue(value);
	group->addData(newLine);
}

} // namespace

GeoDataPolygonGroupCsvImporter::GeoDataPolygonGroupCsvImporter(GeoDataCreator* creator) :
	GeoDataImporter {"csv_polygon_group", tr("CSV file (Polygons)"), creator}
{}

const QStringList GeoDataPolygonGroupCsvImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (Polygons) (*.csv)"));
	return ret;
}

const QStringList GeoDataPolygonGroupCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("csv");
	return ret;
}

bool GeoDataPolygonGroupCsvImporter::importData(GeoData* data, int index, QWidget* w)
{
	auto group = dynamic_cast<GeoDataPolygonGroup*>(data);

	QFile file(filename());
	if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(filename())));
		return false;
	}
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	bool isRef = data->gridAttribute()->isReferenceInformation();

	// check header
	QString buffer;
	bool ok = stream.readLineInto(&buffer);
	if (! ok) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Make sure the file is saved in UTF-8 encoding."));
		return false;
	}
	auto frags = buffer.split(",");
	QStringList headers;
	headers << "pid" << "vid" << "x" << "y" << "name";
	if (isRef) {
		if (frags.size() < 5) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Header should have 5 columns at least."));
			return false;
		}
	} else {
		if (frags.size() < 6) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Header should have 6 columns at least."));
			return false;
		}
		headers << "value";
	}
	for (int i = 0; i < headers.size(); ++i) {
		QString f = frags.at(i).trimmed();
		if (f != headers.at(i)) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Column %1 should be \"%2\", but \"%3\" is given.").arg(i + 1).arg(headers.at(i)).arg(f));
			return false;
		}
	}
	int lineno = 2;

	int currentPid = 0;
	QString currentName;
	double currentValue = 0;
	QPolygonF polygon;

	while (true) {
		bool ok = stream.readLineInto(&buffer);
		if (! ok) {
			if (stream.atEnd()) {
				if (polygon.size() == 0) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. There is no data.").arg(lineno - 1));
					return false;
				}

				if (polygon.size() < 3) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Polygon should have three points at least.").arg(lineno - 1));
					return false;
				}
				addNewPolygon(group, currentName, currentValue, polygon);
				// successfully read all data
				group->setupDataItem();
				group->updateAttributeBrowser(true);
				group->updateVtkObjects();
				group->updateIndex();
				return true;
			}
			// error occured
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Make sure the file is saved in UTF-8 encoding.").arg(lineno));
			return false;
		}
		auto frags = buffer.split(",");
		if (isRef) {
			if (frags.size() < 5) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Data should have 5 columns at least.").arg(lineno));
				return false;
			}
		} else {
			if (frags.size() < 6) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Data should have 6 columns at least.").arg(lineno));
				return false;
			}
			headers << "value";
		}
		int pid, vid;
		double x, y, value = 0;
		QString name;

		pid = frags.at(0).toInt(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"pid\" value in line %1. \"pid\" should be integer value but \"%2\" is specified.").arg(lineno).arg(frags.at(0)));
			return false;
		}
		vid = frags.at(1).toInt(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"vid\" value in line %1. \"vid\" should be integer value but \"%2\" is specified.").arg(lineno).arg(frags.at(1)));
			return false;
		}
		x = frags.at(2).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"x\" value in line %1. \"x\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(2)));
			return false;
		}
		y = frags.at(3).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"y\" value in line %1. \"y\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(3)));
			return false;
		}
		name = frags.at(4);
		if (! isRef) {
			value = frags.at(5).toDouble(&ok);
			if (! ok) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"value\" value in line %1. \"value\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(5)));
				return false;
			}
		}
		if (polygon.size() == 0) {
			// this is the first data
			currentPid = pid;
			currentName = name;
			currentValue = value;
			polygon.push_back(QPointF(x, y));
		} else {
			if (pid != currentPid) {
				// old line finished
				if (polygon.size() < 3) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Polygon should have three points at least.").arg(lineno - 1));
					return false;
				}
				addNewPolygon(group, currentName, currentValue, polygon);

				currentPid = pid;
				currentName = name;
				currentValue = value;
				polygon.clear();
				polygon.push_back(QPointF(x, y));
			} else {
				if (currentName != name) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. \"name\" should be the same for lines where \"pid\" values are the same.").arg(lineno));
					return false;
				}
				if (currentValue != value) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. \"value\" should be the same for lines where \"pid\" values are the same.").arg(lineno));
					return false;
				}
				polygon.push_back(QPointF(x, y));
			}
		}

		++ lineno;
	}
}
