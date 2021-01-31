#include "geodatapointgroup.h"
#include "geodatapointgroupcsvimporter.h"
#include "geodatapointgrouppoint.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

GeoDataPointGroupCsvImporter::GeoDataPointGroupCsvImporter(GeoDataCreator* creator) :
	GeoDataImporter {"csv_point_group", tr("CSV file (Points)"), creator}
{}

const QStringList GeoDataPointGroupCsvImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (Points) (*.csv)"));
	return ret;
}

const QStringList GeoDataPointGroupCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("csv");
	return ret;
}

bool GeoDataPointGroupCsvImporter::importData(GeoData* data, int index, QWidget* w)
{
	auto group = dynamic_cast<GeoDataPointGroup*>(data);

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
	headers << "x" << "y" << "name";
	if (isRef) {
		if (frags.size() < 3) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Header should have 3 columns at least."));
			return false;
		}
	} else {
		if (frags.size() < 4) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading header line. Header should have 4 columns at least."));
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
	while (true) {
		bool ok = stream.readLineInto(&buffer);
		if (! ok) {
			if (stream.atEnd()) {
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
			if (frags.size() < 3) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Data should have 3 columns at least.").arg(lineno));
				return false;
			}
		} else {
			if (frags.size() < 4) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading line %1. Data should have 4 columns at least.").arg(lineno));
				return false;
			}
			headers << "value";
		}
		double x, y, value = 0;
		QString name;
		x = frags.at(0).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"x\" value in line %1. \"x\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(0)));
			return false;
		}
		y = frags.at(1).toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"y\" value in line %1. \"y\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(1)));
			return false;
		}
		name = frags.at(2);
		if (! isRef) {
			value = frags.at(3).toDouble(&ok);
			if (! ok) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured while reading \"value\" value in line %1. \"value\" should be real value but \"%2\" is specified.").arg(lineno).arg(frags.at(3)));
				return false;
			}
		}
		auto point = new GeoDataPointGroupPoint(QPointF(x, y), group);
		point->setName(name);
		point->setValue(value);

		group->addData(point);

		++ lineno;
	}
}
