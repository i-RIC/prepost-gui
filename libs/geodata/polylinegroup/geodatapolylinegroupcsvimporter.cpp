#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupcsvimporter.h"
#include "geodatapolylinegrouppolyline.h"

#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemconvertdialog.h>
#include <cs/coordinatesystemconverter.h>
#include <cs/gdalutil.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

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
	GeoDataImporter {"csv_polyline_group", tr("CSV file (Polylines)"), creator},
	m_converter {nullptr}
{}

GeoDataPolyLineGroupCsvImporter::~GeoDataPolyLineGroupCsvImporter()
{
	delete m_converter;
}

const QStringList GeoDataPolyLineGroupCsvImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (Lines) (*.csv)"));
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

				if (m_converter != nullptr) {
					points = m_converter->convert(points);
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

				if (m_converter != nullptr) {
					points = m_converter->convert(points);
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


bool GeoDataPolyLineGroupCsvImporter::doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	auto projectCs = item->projectData()->mainfile()->coordinateSystem();
	if (projectCs == nullptr) {return true;}

	auto csBuilder = item->projectData()->mainWindow()->coordinateSystemBuilder();
	CoordinateSystemConvertDialog dialog(w);
	dialog.setBuilder(csBuilder);
	dialog.setEnabled(true);

	auto prjFilename = filename;
	prjFilename.replace(QRegExp("\\.csv$"), ".prj");
	if (QFile::exists(prjFilename)) {
		// read and get EPSG code
		QFile f(prjFilename);
		f.open(QFile::ReadOnly);
		auto wkt = f.readAll().toStdString();
		int epsgCode = GdalUtil::wkt2Epsg(wkt.c_str());
		auto cs = csBuilder->system(QString("EPSG:%1").arg(epsgCode));
		dialog.setCoordinateSystem(cs);
	} else {
		dialog.setCoordinateSystem(projectCs);
	}

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return false;
	}
	auto cs = dialog.coordinateSystem();

	if (projectCs != cs) {
		m_converter = new CoordinateSystemConverter(cs, projectCs);
	}
	return true;
}
