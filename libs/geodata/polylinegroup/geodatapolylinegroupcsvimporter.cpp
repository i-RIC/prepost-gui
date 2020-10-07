#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupcsvimporter.h"
#include "geodatapolylinegrouppolyline.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace {

void addNewLine(GeoDataPolyLineGroup* group, const QString& name, double value, const std::vector<QPointF>& points)
{
	auto newLine = new GeoDataPolyLineGroupPolyLine(points, group);
	newLine->setName(name);
	newLine->setValue(value);
	group->addData(newLine);
}

} // namespace

GeoDataPolyLineGroupCsvImporter::GeoDataPolyLineGroupCsvImporter(GeoDataCreator* creator) :
	GeoDataImporter {"csv_polyline_group", tr("CSV file (Polylines)"), creator}
{}

const QStringList GeoDataPolyLineGroupCsvImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (Polylines) (*.csv)"));
	return ret;
}

const QStringList GeoDataPolyLineGroupCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("csv");
	return ret;
}

bool GeoDataPolyLineGroupCsvImporter::importData(GeoData* data, int index, QWidget* w)
{
	auto group = dynamic_cast<GeoDataPolyLineGroup*>(data);

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
	headers << "lid" << "vid" << "x" << "y" << "name";
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

	int currentLid = 0;
	QString currentName;
	double currentValue = 0;
	std::vector<QPointF> points;

	while (true) {
		bool ok = stream.readLineInto(&buffer);
		if (! ok) {
			if (stream.atEnd()) {
				if (points.size() == 0) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. There is no data.").arg(lineno - 1));
					return false;
				}

				if (points.size() == 1) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Line should have two points at least.").arg(lineno - 1));
					return false;
				}
				addNewLine(group, currentName, currentValue, points);
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
		int lid, vid;
		double x, y, value = 0;
		QString name;

		lid = frags.at(0).toInt(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"lid\" value in line %1. \"lid\" should be integer value but \"%2\" is specified.").arg(lineno).arg(frags.at(0)));
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
		if (points.size() == 0) {
			// this is the first data
			currentLid = lid;
			currentName = name;
			currentValue = value;
			points.push_back(QPointF(x, y));
		} else {
			if (lid != currentLid) {
				// old line finished
				if (points.size() == 1) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Line should have two points at least.").arg(lineno - 1));
					return false;
				}
				addNewLine(group, currentName, currentValue, points);

				currentLid = lid;
				currentName = name;
				currentValue = value;
				points.clear();
				points.push_back(QPointF(x, y));
			} else {
				if (currentName != name) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. \"name\" should be the same for lines where \"lid\" values are the same.").arg(lineno));
					return false;
				}
				if (currentValue != value) {
					QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. \"value\" should be the same for lines where \"lid\" values are the same.").arg(lineno));
					return false;
				}
				points.push_back(QPointF(x, y));
			}
		}

		++ lineno;
	}
}
